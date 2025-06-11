#include "H264Converter.h"

// Constructor
H264Converter::H264Converter(std::shared_ptr<DIRECTX12_SHARED> spDirectX12Shared, std::shared_ptr<SharedDX11On12Texture2D> spSharedDX11On12Texture2D, std::shared_ptr<SharedQueue<std::vector<unsigned char>>> spProcessedFrames)
	: spDirectX12Shared(spDirectX12Shared), spSharedDX11On12Texture2D(spSharedDX11On12Texture2D), spProcessedFrames(spProcessedFrames),
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