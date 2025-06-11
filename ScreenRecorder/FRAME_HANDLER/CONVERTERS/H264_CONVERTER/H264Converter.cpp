#include "H264Converter.h"

// Constructor
H264Converter::H264Converter(std::shared_ptr<DIRECTX12_SHARED> spDirectX12Shared, std::shared_ptr<SharedDX11On12Texture2D> spSharedDX11On12Texture2D)
	: spDirectX12Shared(spDirectX12Shared), spSharedDX11On12Texture2D(spSharedDX11On12Texture2D),
		uiCurrentFrameIndex(0), iWidth(0), iHeight(0),																		
			hNvEncodeDll(NULL), pEncoder(NULL), pInputBuffer(NULL), pOutputBuffer(NULL), pRegisteredInputBuffer(NULL), pRegisteredOutputBuffer(NULL)
{
	upNvFuncList = std::make_unique<NV_ENCODE_API_FUNCTION_LIST>();
	upNvEncodeConfig = std::make_unique<NV_ENC_CONFIG>();
	upNvEncInitParams = std::make_unique<NV_ENC_INITIALIZE_PARAMS>();
}

// Destructor
H264Converter::~H264Converter() {
	if (hNvEncodeDll) {
		FreeLibrary(hNvEncodeDll);
	}
	if (pInputBuffer) {
		pInputBuffer->Release();
	}
	if (pOutputBuffer) {
		pOutputBuffer->Release();
	}
}

// Initializer, returns 0 if successfull initialized
// For NVIDIA GPUs - ultra-low latency RAW-to-H264 encode
// Requirements: Windows 10 20H2 and NVIDIA Driver 570+ 
int H264Converter::Initialize(int iNewWidth, int iNewHeight) {
	// Thanks: github.com/NVIDIA/video-sdk-samples

	/* Step 1: check the passed arguments to the function */ 
	if (SetMetrics(iNewWidth, iNewHeight) != 0) {
		// One or two metrics are invalid
		return -1;
	}

	/* Step 2: check spDirectX12Shared components */
	if (!spDirectX12Shared) {
		// spDirectX12Shared is invalid
		return -2;
	}

	/* Step 3: load nvEncodeAPI64.dll */
	hNvEncodeDll = LoadLibraryW(L"nvEncodeAPI64.dll");
	if (!hNvEncodeDll) {
		// Failed to load nvEncodeAPI64.dll
		return -3;
	}

	/* Step 4: initialize NV_ENCODE_API_FUNCTION_LIST */
	if (InitializeNvFunctionList() != 0) {
		// Failed to initialize NV_ENCODE_API_FUNCTION_LIST
		return -4;
	}

	/* Step 5: open encode session */
	if (OpenEncodeSession() != 0) {
		// Failed to open encode session
		return -5;
	}

	/* Step 6: check if H264 Encoder can be used */
	GUID H264EncoderGuid = { 0 };
	if (GetH264Guid(&H264EncoderGuid) != 0) {
		// H264 Encoder cant be used
		return -6;
	}

	/* Step 7: check if the best preset for H264 Encoder can be used */
	GUID EncoderPresetGuid = { 0 };
	if (GetFastestPreset(H264EncoderGuid, &EncoderPresetGuid) != 0) {
		// The fastest encoder preset cant be used
		return -7;
	}

	/* Step 8: get encoder preset configuration */
	std::unique_ptr<NV_ENC_PRESET_CONFIG> upNvPresetConfig = std::make_unique<NV_ENC_PRESET_CONFIG>();
	if (GetFastestConfiguration(H264EncoderGuid, EncoderPresetGuid, upNvPresetConfig.get())) {
		// Failed to get encoder preset configuration
		return -8;
	}
	// Copy NV_ENC_CONFIG from nvPresetConfig to upNvEncodeConfig
	std::memcpy(upNvEncodeConfig.get(), &upNvPresetConfig->presetCfg, sizeof(NV_ENC_CONFIG));

	/* Step 9: select the best encoder profile */
	GUID EncoderProfileGuid = { 0 };
	if (GetFastestProfile(H264EncoderGuid, &EncoderProfileGuid) != 0) {
		// The fastest encoder profile cant be used
		return -9;
	}

	/* Step 10: check, if BGRA format for H264 is supported */
	if (IsBGRASupported(H264EncoderGuid) != 0) {
		// BGRA format is not supported
		return -10;
	}
	
	/* Step 11: initialize NV_ENC_INITIALIZE_PARAMS */
	// It initializes internal field upNvEncInitParams
	if (InitializeEncInitializeParams(H264EncoderGuid, EncoderPresetGuid, EncoderProfileGuid) != 0) {
		// NV_ENC_INITIALIZE_PARAMS cant be initialized
		return -11;
	}

	/* Step 12: initialize the hardware encoder session */
	if (InitializeHardwareEncoderSession() != 0) {
		// Failed to initialize the hardware encoder session
		return -12;
	}

	/* Step 13: register input/output buffers */
	if (RegisterBuffers() != 0) {
		// Failed to register input/output buffers
		return -13;
	}

	// H264Converter inited!
	return 0;
}

// Converts raw BGRA frame to .jpeg format. Returns empty vector if error is occured
// This function is called when ID3D12Resource* pInputBuffer has valid BGRA frame
std::vector<unsigned char> H264Converter::Convert(const std::vector<unsigned char>& vRawFrame) {
	NVENCSTATUS nvStatus;

	/* Step 1: map input and output buffers */
	// 1. Map input buffer
	std::unique_ptr<NV_ENC_MAP_INPUT_RESOURCE> upMapInRes = std::make_unique<NV_ENC_MAP_INPUT_RESOURCE>();
	std::memset(upMapInRes.get(), 0, sizeof(NV_ENC_MAP_INPUT_RESOURCE));
	
	// Define fields of NV_ENC_MAP_INPUT_RESOURCE
	upMapInRes->version = NV_ENC_MAP_INPUT_RESOURCE_VER;
	upMapInRes->registeredResource = pRegisteredInputBuffer;
	
	// Map resource
	nvStatus = upNvFuncList->nvEncMapInputResource(pEncoder, upMapInRes.get());
	if (NVFAILED(nvStatus)) {
		// Error to encode frame
		return std::vector<unsigned char>();
	}

	// 1. Map input buffer
	std::unique_ptr<NV_ENC_MAP_INPUT_RESOURCE> upMapOutRes = std::make_unique<NV_ENC_MAP_INPUT_RESOURCE>();
	std::memset(upMapOutRes.get(), 0, sizeof(NV_ENC_MAP_INPUT_RESOURCE));

	// Define fields of NV_ENC_MAP_INPUT_RESOURCE
	upMapOutRes->version = NV_ENC_MAP_INPUT_RESOURCE_VER;
	upMapOutRes->registeredResource = pRegisteredOutputBuffer;

	// Map resource
	nvStatus = upNvFuncList->nvEncMapInputResource(pEncoder, upMapOutRes.get());
	if (NVFAILED(nvStatus)) {
		// Error to encode frame
		return std::vector<unsigned char>();
	}

	/* Step 2: submitting input frame for encoding */
	// 1. Create NV_ENC_INPUT_RESOURCE_D3D12
	std::unique_ptr<NV_ENC_INPUT_RESOURCE_D3D12> upInResD3D12 = std::make_unique<NV_ENC_INPUT_RESOURCE_D3D12>();
	std::memset(upInResD3D12.get(), 0, sizeof(NV_ENC_INPUT_RESOURCE_D3D12));

	// Define fields of NV_ENC_INPUT_RESOURCE_D3D12
	upInResD3D12->version = NV_ENC_INPUT_RESOURCE_D3D12_VER;
	upInResD3D12->pInputBuffer = upMapInRes->mappedResource;
	std::memcpy(&upInResD3D12->inputFencePoint, upInFencePoint.get(), sizeof(NV_ENC_FENCE_POINT_D3D12));

	// 2. Create NV_ENC_OUTPUT_RESOURCE_D3D12
	std::unique_ptr<NV_ENC_OUTPUT_RESOURCE_D3D12> upOutResD3D12 = std::make_unique<NV_ENC_OUTPUT_RESOURCE_D3D12>();
	std::memset(upOutResD3D12.get(), 0, sizeof(NV_ENC_OUTPUT_RESOURCE_D3D12));

	// Define fields of NV_ENC_INPUT_RESOURCE_D3D12
	upOutResD3D12->version = NV_ENC_INPUT_RESOURCE_D3D12_VER;
	upOutResD3D12->pOutputBuffer = upMapOutRes->mappedResource;
	std::memcpy(&upOutResD3D12->outputFencePoint, upOutFencePoint.get(), sizeof(NV_ENC_FENCE_POINT_D3D12));


	// 2. Create NV_ENC_PIC_PARAMS
	std::unique_ptr<NV_ENC_PIC_PARAMS> upNvEncPicParams = std::make_unique<NV_ENC_PIC_PARAMS>();
	std::memset(upNvEncPicParams.get(), 0, sizeof(NV_ENC_PIC_PARAMS));

	// Define fields of NV_ENC_PIC_PARAMS
	upNvEncPicParams->version = NV_ENC_PIC_PARAMS_VER;
	upNvEncPicParams->inputWidth = iWidth;
	upNvEncPicParams->inputHeight = iHeight;
	upNvEncPicParams->inputPitch = 0;
	upNvEncPicParams->encodePicFlags = 0;
	upNvEncPicParams->frameIdx = uiCurrentFrameIndex;
	upNvEncPicParams->inputTimeStamp = uiCurrentFrameIndex * 166667;
	upNvEncPicParams->inputDuration = 166667;
	upNvEncPicParams->inputBuffer = upInResD3D12.get();
	upNvEncPicParams->outputBitstream = upOutResD3D12.get();
	upNvEncPicParams->completionEvent = nullptr;
	upNvEncPicParams->bufferFmt = NV_ENC_BUFFER_FORMAT_ARGB;
	upNvEncPicParams->pictureStruct = NV_ENC_PIC_STRUCT_FRAME;
	upNvEncPicParams->pictureType = NV_ENC_PIC_TYPE_P;

	// Encode frame in ID3D12Resource* pInputBuffer and save in pOutputBuffer
	nvStatus = upNvFuncList->nvEncEncodePicture(pEncoder, upNvEncPicParams.get());
	if (NVFAILED(nvStatus)) {
		// Error to encode frame
		return std::vector<unsigned char>();
	}

	/* Step 3: retrieving encoded output */
	// 1. Get a CPU pointer to the encoded bitstream
	std::unique_ptr<NV_ENC_LOCK_BITSTREAM> upLockBitstream = std::make_unique<NV_ENC_LOCK_BITSTREAM>();
	std::memset(upLockBitstream.get(), 0, sizeof(NV_ENC_LOCK_BITSTREAM));

	// Define fields of NV_ENC_LOCK_BITSTREAM
	upLockBitstream->version = NV_ENC_LOCK_BITSTREAM_VER;
	upLockBitstream->doNotWait = 0;
	upLockBitstream->outputBitstream = upOutResD3D12.get();

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	// Call function to get a CPU pointer to the encoded bitstream
	nvStatus = upNvFuncList->nvEncLockBitstream(pEncoder, upLockBitstream.get());
	if (NVFAILED(nvStatus)) {
		// Error to get a CPU pointer to the encoded bitstream
		return std::vector<unsigned char>();
	}

	/* Step 4: copy buffer into vector and release/unmap all data */
	// 1. Copy H.264 encoded frame into the vector
	std::vector<unsigned char> vH264Frame(reinterpret_cast<char*>(upLockBitstream->bitstreamBufferPtr), reinterpret_cast<char*>(upLockBitstream->bitstreamBufferPtr) + upLockBitstream->bitstreamSizeInBytes);

	// 2. Unlock output bitstream
	upNvFuncList->nvEncUnlockBitstream(pEncoder, upLockBitstream->outputBitstream);

	// 3. Unmap input and output buffers
	upNvFuncList->nvEncUnmapInputResource(pEncoder, upMapInRes->mappedResource);
	upNvFuncList->nvEncUnmapInputResource(pEncoder, upMapOutRes->mappedResource);
	
	// Don`t forget to increment frame index uiCurrentFrameIndex
	uiCurrentFrameIndex++;

	return vH264Frame;
}