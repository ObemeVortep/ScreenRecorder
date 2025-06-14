#include "H264Converter.h"

// Map buffer and return unique_ptr<NV_ENC_MAP_INPUT_RESOURCE>
std::unique_ptr<NV_ENC_MAP_INPUT_RESOURCE> H264Converter::MapNvBuffer(NV_ENC_REGISTERED_PTR pRegisteredResource) {
	NVENCSTATUS nvStatus;

	// Allocate new unique_ptr with NV_ENC_MAP_INPUT_RESOURCE
	auto mapInputRes = std::make_unique<NV_ENC_MAP_INPUT_RESOURCE>();
	std::memset(mapInputRes.get(), 0, sizeof(NV_ENC_MAP_INPUT_RESOURCE));
	
	// Define fields
	mapInputRes->version = NV_ENC_MAP_INPUT_RESOURCE_VER;
	mapInputRes->registeredResource = pRegisteredResource;

	// Try to map resource
	nvStatus = upNvFuncList->nvEncMapInputResource(pEncoder, mapInputRes.get());
	if (NVFAILED(nvStatus)) {
		// Error to map resource
		return nullptr;
	}

	return mapInputRes;
}

// Create and return unique_ptr<NV_ENC_INPUT_RESOURCE_D3D12> or unique_ptr<NV_ENC_OUTPUT_RESOURCE_D3D12>
template<NvEncInOutResorceD3D12 T>
std::unique_ptr<T> H264Converter::CreateNvResD3D12(NV_ENC_INPUT_PTR pMappedResource, NV_ENC_FENCE_POINT_D3D12* pFenceD3D12) {
	// Allocate new unique_ptr with T class (can be either NV_ENC_INPUT_RESOURCE_D3D12 or NV_ENC_OUTPUT_RESOURCE_D3D12)
	auto res = std::make_unique<T>();
	std::memset(res.get(), 0, sizeof(T));

	// Depending on the type, we fill it differently
	if constexpr (std::same_as<T, NV_ENC_INPUT_RESOURCE_D3D12>) {
		res->version = NV_ENC_INPUT_RESOURCE_D3D12_VER;
		res->pInputBuffer = pMappedResource;
		std::memcpy(&res->inputFencePoint, pFenceD3D12, sizeof(NV_ENC_FENCE_POINT_D3D12));
	}
	else {
		res->version = NV_ENC_OUTPUT_RESOURCE_D3D12_VER;
		res->pOutputBuffer = pMappedResource;
		std::memcpy(&res->outputFencePoint, pFenceD3D12, sizeof(NV_ENC_FENCE_POINT_D3D12));
	}

	return res;
}

// Create and return NV_ENC_PIC_PARAMS in unique_ptr
inline std::unique_ptr<NV_ENC_PIC_PARAMS> H264Converter::CreateNvPicParams(NV_ENC_INPUT_PTR pInputBuffer, NV_ENC_OUTPUT_PTR pOutputBitstream) {
	// Allocate new NV_ENC_PIC_PARAMS in unique_ptr 
	auto upNvEncPicParams = std::make_unique<NV_ENC_PIC_PARAMS>();
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
	upNvEncPicParams->inputBuffer = pInputBuffer;
	upNvEncPicParams->outputBitstream = pOutputBitstream;
	upNvEncPicParams->completionEvent = nullptr;
	upNvEncPicParams->bufferFmt = NV_ENC_BUFFER_FORMAT_ARGB;
	upNvEncPicParams->pictureStruct = NV_ENC_PIC_STRUCT_FRAME;
	upNvEncPicParams->pictureType = NV_ENC_PIC_TYPE_P;

	return upNvEncPicParams;
}

// Create and return NV_ENC_LOCK_BITSTREAM in unique_ptr
inline std::unique_ptr<NV_ENC_LOCK_BITSTREAM> H264Converter::CreateNvLockBitsream(PVOID pOutputBistream) {
	// Allocate NV_ENC_LOCK_BITSTREAM in unique_ptr
	auto lockBitstream = std::make_unique<NV_ENC_LOCK_BITSTREAM>();
	std::memset(lockBitstream.get(), 0, sizeof(NV_ENC_LOCK_BITSTREAM));

	// Define fields of NV_ENC_LOCK_BITSTREAM
	lockBitstream->version = NV_ENC_LOCK_BITSTREAM_VER;
	lockBitstream->doNotWait = 0;
	lockBitstream->outputBitstream = pOutputBistream;

	return lockBitstream;
}

// Converts raw BGRA frame to .jpeg format. Returns empty vector if error is occured
// This function is called when ID3D12Resource* pInputBuffer has valid BGRA frame
std::vector<unsigned char> H264Converter::Convert() {
	NVENCSTATUS nvStatus;

	/* Step 1: map input and output buffers */
	// 1. Map input buffer
	auto upMapInRes = MapNvBuffer(pRegisteredInputBuffer);
	if (!upMapInRes) {
		// Error to map input buffer
		return std::vector<unsigned char>();
	}

	// 2. Map output buffer
	auto upMapOutRes = MapNvBuffer(pRegisteredOutputBuffer);
	if (!upMapOutRes) {
		// Error to map output buffer
		return std::vector<unsigned char>();
	}

	/* Step 2: create NV_ENC_INPUT_RESOURCE_D3D12 and NV_ENC_OUTPUT_RESOURCE_D3D12 */
	auto upInResD3D12 = CreateNvResD3D12<NV_ENC_INPUT_RESOURCE_D3D12>(upMapInRes->mappedResource, upInFencePoint.get());
	auto upOutResD3D12 = CreateNvResD3D12<NV_ENC_OUTPUT_RESOURCE_D3D12>(upMapOutRes->mappedResource, upOutFencePoint.get());

	/* Step 3: create NV_ENC_PIC_PARAMS */
	auto upNvEncPicParams = CreateNvPicParams(upInResD3D12.get(), upOutResD3D12.get());

	/* Step 4: encode frame in ID3D12Resource* pInputBuffer and save in pOutputBuffer */ 
	nvStatus = upNvFuncList->nvEncEncodePicture(pEncoder, upNvEncPicParams.get());
	if (NVFAILED(nvStatus)) {
		// Error to encode frame
		return std::vector<unsigned char>();
	}

	/* Step 3: retrieving encoded output */
	// 1. Create NV_ENC_LOCK_BITSTREAM for access to the encoded bitstream
	auto upLockBitstream = CreateNvLockBitsream(upOutResD3D12.get());

	// 2. Call function to get a CPU pointer to the encoded bitstream
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