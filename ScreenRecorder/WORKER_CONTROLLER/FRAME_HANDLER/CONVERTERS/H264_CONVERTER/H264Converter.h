#ifndef H264_CONVERTER_H
#define H264_CONVERTER_H

#include "INTERFACES/IConverter.h"

#include "TYPES/DirectXShared.h"
#include "TYPES/SharedDX11On12Texture2D.h"
#include "TYPES/SharedQueue.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d12.h>
#include "nvidia-nvenc/nvEncodeAPI.h"
#include "nvidia-nvenc/nvcommon.h"
#include "H264Concepts.h"

#include <memory>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#define FRAMES_PER_SECOND   60      // Frames per second, that we recording

// Concept that 

class H264Converter : public IConverter {
public:
	// Constructor
	H264Converter(std::shared_ptr<DIRECTX12_SHARED> spDirectX12Shared, std::shared_ptr<SharedDX11On12Texture2D> spSharedDX11On12Texture2D, std::shared_ptr<SharedQueue<std::vector<unsigned char>>> spProcessedFrames);
	// Destructor 
	~H264Converter();

	// Returns 0 if successfull initialized
	int Initialize(int iNewWidth, int iNewHeight) override;

	// Convert raw BGRA frame to H.264 format
	// This function is called when ID3D12Resource* pInputBuffer has valid BGRA frame
	std::vector<unsigned char> Convert() override;

private:
	// Initialize helper functions
	// Set metrics
	int SetMetrics(int iNewWidth, int iNewHeight);

	// Initialize NV_ENCODE_API_FUNCTION_LIST (upNvFuncList)
	int InitializeNvFunctionList();

	// Open encode session and initialize pEncoder
	int OpenEncodeSession();

	// Get GUID of H.264 Encoder and returns 0 on success
	int GetH264Guid(GUID* pH264EncoderGuid);

	// Get the fastest H.264 Encoder preset that we can use and returns his index
	int GetFastestPreset(const GUID& H264EncoderGuid, GUID* pPresetGuid);

	// Get the fastest configuration for H.264 Encoder
	int GetFastestConfiguration(const GUID& H264EncoderGuid, const GUID& PresetGuid, NV_ENC_PRESET_CONFIG* pNvPresetConfig);

	// Get the fastest profile for H.264 Encoder
	int GetFastestProfile(const GUID& H264EncoderGuid, GUID* pEncoderProfileGuid);

	// Check, if BGRA format is supported for encoder
	int IsBGRASupported(const GUID& H264EncoderGuid);

	// Initialize NV_ENC_INITIALIZE_PARAMS for opening hardware encoder session
	int InitializeEncInitializeParams(const GUID& H264EncoderGuid, const GUID& PresetGuid, const GUID& ProfileGuid);

	// Initialize the hardware encoder session
	int InitializeHardwareEncoderSession();

	// Register input/output buffers in NVIDIA NVENC
	int RegisterBuffers();
	int RegisterInputBuffer();
	int RegisterOutputBuffer();

private:
	// Convert helper functions
	// Map buffer and return NV_ENC_MAP_INPUT_RESOURCE in unique_ptr
	inline std::unique_ptr<NV_ENC_MAP_INPUT_RESOURCE> MapNvBuffer(NV_ENC_REGISTERED_PTR pRegisteredResource);

	// Create and return NV_ENC_INPUT_RESOURCE_D3D12 or NV_ENC_OUTPUT_RESOURCE_D3D12 in unique_ptr
	template <NvEncInOutResorceD3D12 T>
	inline std::unique_ptr<T> CreateNvResD3D12(NV_ENC_INPUT_PTR pMappedResource, NV_ENC_FENCE_POINT_D3D12* pFenceD3D12);

	// Create and return NV_ENC_PIC_PARAMS in unique_ptr
	inline std::unique_ptr<NV_ENC_PIC_PARAMS> CreateNvPicParams(NV_ENC_INPUT_PTR pInputBuffer, NV_ENC_OUTPUT_PTR pOutputBitstream);
	
	// Create and return NV_ENC_LOCK_BITSTREAM in unique_ptr
	inline std::unique_ptr<NV_ENC_LOCK_BITSTREAM> CreateNvLockBitsream(PVOID pOutputBistream);

private:
	// DirectX structures, that are shared with other modules for optimizing performance
	std::shared_ptr<DIRECTX12_SHARED> spDirectX12Shared;

private:
	// Connectors between RECORDER <-> HANDLER
	// ScreenRecorder -> FrameHandler
	std::shared_ptr<SharedDX11On12Texture2D> spSharedDX11On12Texture2D;

private:
	// Connectors between HANDLER -> VIDEO_CREATOR
	// FrameHandler -> VideoCreator
	std::shared_ptr<SharedQueue<std::vector<unsigned char>>> spProcessedFrames;

private:
	// Variables that use while encoding
	// Current frame index
	uint32_t uiCurrentFrameIndex;

private:
	// Variables

	// Screen metrics
	int iWidth;
	int iHeight;

	// For NVIDIA GPUs (ultra low latency encoding RAW-to-H.264)
	HMODULE hNvEncodeDll;
	PVOID pEncoder;
	std::unique_ptr<NV_ENCODE_API_FUNCTION_LIST> upNvFuncList;
	std::unique_ptr<NV_ENC_CONFIG> upNvEncodeConfig;
	std::unique_ptr<NV_ENC_INITIALIZE_PARAMS> upNvEncInitParams;

	ComPtr<ID3D12Fence> cpInputFence;
	std::unique_ptr<NV_ENC_FENCE_POINT_D3D12> upInFencePoint;

	ComPtr<ID3D12Fence> cpOutputFence;
	std::unique_ptr<NV_ENC_FENCE_POINT_D3D12> upOutFencePoint;

	ID3D12Resource* pInputBuffer;
	ID3D12Resource* pOutputBuffer;

	PVOID pRegisteredInputBuffer;
	PVOID pRegisteredOutputBuffer;
};

#endif // H264_CONVERTER_H