#include "H264Converter.h"

// Set metrics
int H264Converter::SetMetrics(int iNewWidth, int iNewHeight) {
	// Check if metrics are valid
	if (iNewWidth <= 0 || iNewHeight <= 0) {
		// One or two metrics are invalid
		return -1;
	}
	iWidth = iNewWidth;
	iHeight = iNewHeight;
	
	return 0;
}

// Initialize NV_ENCODE_API_FUNCTION_LIST (upNvFuncList)
int H264Converter::InitializeNvFunctionList() {
	NVENCSTATUS nvStatus;

	// 1. Get NvEncodeAPICreateInstance proc address
	auto pNvEncodeAPICreateInstance = (NvEncodeAPICreateInstance_t)GetProcAddress(hNvEncodeDll, "NvEncodeAPICreateInstance");
	if (!pNvEncodeAPICreateInstance) {
		// Failed to get NvEncodeAPICreateInstance address
		return -1;
	}

	// Initialize NV_ENCODE_API_FUNCTION_LIST
	upNvFuncList->version = NV_ENCODE_API_FUNCTION_LIST_VER;

	// Call NvEncodeAPICreateInstance
	nvStatus = pNvEncodeAPICreateInstance(upNvFuncList.get());
	if (NVFAILED(nvStatus)) {
		// Failed to create nvEncodeInstance
		return -2;
	}

	return 0;
}

// Open encode session and initialize pEncoder
int H264Converter::OpenEncodeSession() {
	NVENCSTATUS nvStatus;

	// Initialize NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS
	NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS nvEncSessionParams = { 0 };
	nvEncSessionParams.version = NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS_VER;
	nvEncSessionParams.deviceType = NV_ENC_DEVICE_TYPE_DIRECTX;
	nvEncSessionParams.device = (PVOID)spDirectX12Shared->cpDevice.Get();
	nvEncSessionParams.apiVersion = NVENCAPI_VERSION;

	// Open encode session
	nvStatus = upNvFuncList->nvEncOpenEncodeSessionEx(&nvEncSessionParams, &pEncoder);
	if (NVFAILED(nvStatus)) {
		// Failed to open encode session
		return -1;
	}

	return 0;
}

// Function set GUID of H.264 Encoder and returns 0 on success
int H264Converter::GetH264Guid(GUID* pH264EncoderGuid) {
	NVENCSTATUS nvStatus;

	// 1. Get count of GUIDs
	uint32_t uiGuidCount = 0;
	nvStatus = upNvFuncList->nvEncGetEncodeGUIDCount(pEncoder, &uiGuidCount);
	if (NVFAILED(nvStatus)) {
		// Failed to get count of GUID
		return -1;
	}

	// 2. Get exact GUIDs
	uint32_t uiExactGuids = 0;
	std::vector<GUID> vGuids(uiGuidCount);
	nvStatus = upNvFuncList->nvEncGetEncodeGUIDs(pEncoder, vGuids.data(), uiGuidCount, &uiExactGuids);
	if (NVFAILED(nvStatus)) {
		// Failed to get exact GUIDs
		return -2;
	}

	// Check if H264 codec supported 
	for (uint32_t i = 0; i < uiExactGuids; i++) {
		if (vGuids[i] == NV_ENC_CODEC_H264_GUID) {
			// H264 Encoder exists and can be used.
			*pH264EncoderGuid = NV_ENC_CODEC_H264_GUID;
			return 0;
		}
	}

	// H264 was not found. Return with error.
	return -3;
}

// Function set the fastest encoder preset that we can use and returns his index
int H264Converter::GetFastestPreset(const GUID& H264EncoderGuid, GUID* pEncoderPresetGuid) {
	NVENCSTATUS nvStatus;

	// 1. Get count of H264 Encoder presets
	uint32_t uiPresetCount = 0;
	nvStatus = upNvFuncList->nvEncGetEncodePresetCount(pEncoder, H264EncoderGuid, &uiPresetCount);
	if (NVFAILED(nvStatus)) {
		// Failed to get count of H264 Encoder presets
		return -1;
	}

	// 2. Get exact presets of H264 Encoder
	uint32_t uiExactPresets = 0;
	std::vector<GUID> vPresets(uiPresetCount);
	nvStatus = upNvFuncList->nvEncGetEncodePresetGUIDs(pEncoder, H264EncoderGuid, vPresets.data(), uiPresetCount, &uiExactPresets);
	if (NVFAILED(nvStatus)) {
		// Failed to get exact H264 Encoder presets
		return -2;
	}
	// Selecting the fastest encoder preset (NV_ENC_PRESET_P1_GUID)
	for (uint32_t i = 0; i < uiExactPresets; i++) {
		if (vPresets[i] == NV_ENC_PRESET_P1_GUID) {
			// NV_ENC_PRESET_P1_GUID can be used.
			*pEncoderPresetGuid = NV_ENC_PRESET_P1_GUID;
			return 0;
		}
	}

	// NV_ENC_PRESET_P1_GUID was not found. Return with error.
	return -3;
}

// Get the fastest configuration for encoder
int H264Converter::GetFastestConfiguration(const GUID& H264EncoderGuid, const GUID& PresetGuid, NV_ENC_PRESET_CONFIG* pNvPresetConfig) {
	NVENCSTATUS nvStatus;

	// Zeroing out NV_ENC_PRESET_CONFIG
	std::memset(pNvPresetConfig, 0, sizeof(NV_ENC_PRESET_CONFIG));

	// Define version of configs 
	pNvPresetConfig->version = NV_ENC_PRESET_CONFIG_VER;
	pNvPresetConfig->presetCfg.version = NV_ENC_CONFIG_VER;

	// Try to get the fastest configuration for Encoder
	nvStatus = upNvFuncList->nvEncGetEncodePresetConfigEx(pEncoder, H264EncoderGuid, PresetGuid, NV_ENC_TUNING_INFO_ULTRA_LOW_LATENCY, pNvPresetConfig);
	if (NVFAILED(nvStatus)) {
		// Failed to get the fastest configuration for Encoder
		return -1;
	}

	return 0;
}

// Get the faster profile for encoder
int H264Converter::GetFastestProfile(const GUID& H264EncoderGuid, GUID* pEncoderProfileGuid) {
	NVENCSTATUS nvStatus;

	// 1. Get count of H264 Encoder profiles
	uint32_t uiProfileCount = 0;
	nvStatus = upNvFuncList->nvEncGetEncodeProfileGUIDCount(pEncoder, H264EncoderGuid, &uiProfileCount);
	if (NVFAILED(nvStatus)) {
		// Failed to get count of H264 Encoder profiles
		return -1;
	}

	// 2. Get exact profiles of H264 Encoder
	uint32_t uiExactProfiles = 0;
	std::vector<GUID> vProfiles(uiProfileCount);
	nvStatus = upNvFuncList->nvEncGetEncodeProfileGUIDs(pEncoder, H264EncoderGuid, vProfiles.data(), uiProfileCount, &uiExactProfiles);
	if (NVFAILED(nvStatus)) {
		// Failed to get exact H264 Encoder profiles
		return -2;
	}
	// Selecting the fastest encoder profile (NV_ENC_H264_PROFILE_BASELINE_GUID)
	for (uint32_t i = 0; i < uiExactProfiles; i++) {
		if (vProfiles[i] == NV_ENC_H264_PROFILE_BASELINE_GUID) {
			// NV_ENC_H264_PROFILE_BASELINE_GUID can be used.
			*pEncoderProfileGuid = NV_ENC_H264_PROFILE_BASELINE_GUID;
			return 0;
		}
	}

	// NV_ENC_H264_PROFILE_BASELINE_GUID was not found. Return with error.
	return -3;
}

// Check, if BGRA format is supported for encoder
int H264Converter::IsBGRASupported(const GUID& H264EncoderGuid) {
	NVENCSTATUS nvStatus;

	// 1. Get count H264 Encoder supported buffer formats
	uint32_t uiFormatCount = 0;
	nvStatus = upNvFuncList->nvEncGetInputFormatCount(pEncoder, H264EncoderGuid, &uiFormatCount);
	if (NVFAILED(nvStatus)) {
		// Failed to get count H264 Encoder supported formats
		return -1;
	}

	// 2. Get exact H264 Encoder supported buffer formats 
	uint32_t uiExactFormats = 0;
	std::vector<NV_ENC_BUFFER_FORMAT> vFormats(uiFormatCount);
	nvStatus = upNvFuncList->nvEncGetInputFormats(pEncoder, H264EncoderGuid, vFormats.data(), uiFormatCount, &uiExactFormats);
	if (NVFAILED(nvStatus)) {
		// Failed to get exact H264 Encoder supported formats
		return -2;
	}
	// Parsing und check, if BGRA format (NV_ENC_BUFFER_FORMAT_ARGB) is supported
	for (uint32_t i = 0; i < uiExactFormats; i++) {
		if (vFormats[i] == NV_ENC_BUFFER_FORMAT_ARGB) {
			// NV_ENC_BUFFER_FORMAT_ARGB is supported
			return 0;
		}
	}

	// NV_ENC_BUFFER_FORMAT_ARGB is not supported
	return -3;
}

// Initialize NV_ENC_INITIALIZE_PARAMS for opening hardware encoder session
int H264Converter::InitializeEncInitializeParams(const GUID& H264EncoderGuid, const GUID& PresetGuid, const GUID& ProfileGuid) {
	NVENCSTATUS nvStatus;

	// Zeroing out NV_ENC_INITIALIZE_PARAMS
	std::memset(upNvEncInitParams.get(), 0, sizeof(NV_ENC_INITIALIZE_PARAMS));
	
	// 1. Initialize fields of NV_ENC_INITIALIZE_PARAMS
	upNvEncInitParams->version = NV_ENC_INITIALIZE_PARAMS_VER;
	upNvEncInitParams->encodeGUID = H264EncoderGuid;
	upNvEncInitParams->presetGUID = PresetGuid;
	upNvEncInitParams->encodeWidth = static_cast<uint32_t>(iWidth);
	upNvEncInitParams->encodeHeight = static_cast<uint32_t>(iHeight);
	upNvEncInitParams->darWidth = static_cast<uint32_t>(iWidth);
	upNvEncInitParams->darHeight = static_cast<uint32_t>(iHeight);
	upNvEncInitParams->frameRateNum = 60;
	upNvEncInitParams->frameRateDen = 1;
	upNvEncInitParams->enablePTD = 1;
	upNvEncInitParams->reportSliceOffsets = 0;
	upNvEncInitParams->enableSubFrameWrite = 0;
	upNvEncInitParams->maxEncodeWidth = static_cast<uint32_t>(iWidth);
	upNvEncInitParams->maxEncodeHeight = static_cast<uint32_t>(iHeight);
	upNvEncInitParams->tuningInfo = NV_ENC_TUNING_INFO_ULTRA_LOW_LATENCY;
	upNvEncInitParams->bufferFormat = NV_ENC_BUFFER_FORMAT_ARGB;

	// 2. Initialize encodeConfig and his internal fields
	upNvEncInitParams->encodeConfig = upNvEncodeConfig.get();
	upNvEncInitParams->encodeConfig->version = NV_ENC_CONFIG_VER;
	upNvEncInitParams->encodeConfig->profileGUID = ProfileGuid;
	upNvEncInitParams->encodeConfig->gopLength = NVENC_INFINITE_GOPLENGTH;
	upNvEncInitParams->encodeConfig->frameIntervalP = 1;
	upNvEncInitParams->encodeConfig->rcParams.rateControlMode = NV_ENC_PARAMS_RC_CONSTQP;
	upNvEncInitParams->encodeConfig->encodeCodecConfig.h264Config.idrPeriod = upNvEncInitParams->encodeConfig->gopLength;
	
	return 0;
}

// Initialize the hardware encoder session
int H264Converter::InitializeHardwareEncoderSession() {
	NVENCSTATUS nvStatus;

	// NvEncInitializeEncoder initializes the encoder
	nvStatus = upNvFuncList->nvEncInitializeEncoder(pEncoder, upNvEncInitParams.get());
	if (NVFAILED(nvStatus)) {
		// NvEncInitializeEncoder has failed to initialize the encoder
		return -1;
	}

	return 0;
}

// Register input and output buffers in NVIDIA NVENC
int H264Converter::RegisterBuffers() {
	/* Step 1: register input buffer */
	if (RegisterInputBuffer() != 0) {
		std::cerr << "INPUT PROBLEM" << std::endl;
		return -1;
	}

	/* Step 1: register output buffer */
	if (RegisterOutputBuffer() != 0) {
		std::cerr << "OUTPUT PROBLEM" << std::endl;
		return -2;
	}

	return 0;
}
int H264Converter::RegisterInputBuffer() {
	HRESULT hr;
	NVENCSTATUS nvStatus;

	/* Step 1: create ID3D12Resource from ID3D11Resource */
	hr = spSharedDX11On12Texture2D->CreateD12ResourceFromD11(reinterpret_cast<void**>(&pInputBuffer));
	if (FAILED(hr)) {
		// Failed to create ID3D12Resource from ID3D11Resource
		return -1;
	}

	/* Step 2: create ID3D12Fence for input buffer */
	hr = spDirectX12Shared->cpDevice.Get()->CreateFence(
		0,
		D3D12_FENCE_FLAG_NONE,
		__uuidof(ID3D12Fence),
		reinterpret_cast<void**>(cpInputFence.GetAddressOf())
	);
	if (FAILED(hr)) {
		// Failed to create ID3D12Fence for input buffer
		return -2;
	}

	/* Step 3: register input buffer in NVIDIA NVENC */
	std::unique_ptr<NV_ENC_REGISTER_RESOURCE> upNvRegRes = std::make_unique<NV_ENC_REGISTER_RESOURCE>();
	upInFencePoint = std::make_unique<NV_ENC_FENCE_POINT_D3D12>();
	std::memset(upNvRegRes.get(), 0, sizeof(NV_ENC_REGISTER_RESOURCE));
	std::memset(upInFencePoint.get(), 0, sizeof(NV_ENC_FENCE_POINT_D3D12));

	// Define fields of NV_ENC_REGISTER_RESOURCE
	upNvRegRes->version = NV_ENC_REGISTER_RESOURCE_VER;
	upNvRegRes->resourceType = NV_ENC_INPUT_RESOURCE_TYPE_DIRECTX;
	upNvRegRes->width = iWidth;
	upNvRegRes->height = iHeight;
	upNvRegRes->pitch = 0;
	upNvRegRes->subResourceIndex = 0;
	upNvRegRes->resourceToRegister = pInputBuffer;
	upNvRegRes->bufferFormat = NV_ENC_BUFFER_FORMAT_ARGB;
	upNvRegRes->bufferUsage = NV_ENC_INPUT_IMAGE;
	upNvRegRes->pInputFencePoint = upInFencePoint.get();

	// Define fields of NV_ENC_FENCE_POINT_D3D12
	upInFencePoint->version = NV_ENC_FENCE_POINT_D3D12_VER;
	upInFencePoint->pFence = cpInputFence.Get();
	upInFencePoint->waitValue = 10;
	upInFencePoint->signalValue = 10;
	upInFencePoint->bWait = 0;
	upInFencePoint->bSignal = 0;

	nvStatus = upNvFuncList->nvEncRegisterResource(pEncoder, upNvRegRes.get());
	if (NVFAILED(nvStatus)) {
		// Failed to register input buffer in NVIDIA NVENC
		std::cerr << "NVSTATUS: 0x" << nvStatus << std::endl;
		return -3;
	}

	// Save handle of registered input buffer
	pRegisteredInputBuffer = upNvRegRes->registeredResource;

	return 0;
}
int H264Converter::RegisterOutputBuffer() {
	HRESULT hr;
	NVENCSTATUS nvStatus;

	/* Step 1: initialize output buffer */
	D3D12_HEAP_PROPERTIES BufferProperties;
	D3D12_RESOURCE_DESC ResourceDesc;

	// Define new properties
	BufferProperties.Type = D3D12_HEAP_TYPE_READBACK;
	BufferProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	BufferProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	BufferProperties.CreationNodeMask = 0;
	BufferProperties.VisibleNodeMask = 0;

	// Define new description
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	ResourceDesc.Width = iWidth * iHeight * 4 * 2;
	ResourceDesc.Height = 1;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	ResourceDesc.SampleDesc.Count = 1;
	ResourceDesc.SampleDesc.Quality = 0;
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// Allocate output buffer using D3D12 Device
	hr = spDirectX12Shared->cpDevice.Get()->CreateCommittedResource(
		&BufferProperties,
		D3D12_HEAP_FLAG_NONE,
		&ResourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		__uuidof(ID3D12Resource),
		reinterpret_cast<void**>(&pOutputBuffer)
	);
	if (FAILED(hr)) {
		// Failed to create output buffer
		return -1;
	}

	/* Step 3: create ID3D12Fence for output buffer */
	hr = spDirectX12Shared->cpDevice.Get()->CreateFence(
		0,
		D3D12_FENCE_FLAG_NONE,
		__uuidof(ID3D12Fence),
		reinterpret_cast<void**>(cpOutputFence.GetAddressOf())
	);
	if (FAILED(hr)) {
		// Failed to create ID3D12Fence for input buffer
		return -2;
	}

	/* Step 3: register output buffer in NVIDIA NVENC */
	std::unique_ptr<NV_ENC_REGISTER_RESOURCE> upNvRegRes = std::make_unique<NV_ENC_REGISTER_RESOURCE>();
	upOutFencePoint = std::make_unique<NV_ENC_FENCE_POINT_D3D12>();
	std::memset(upNvRegRes.get(), 0, sizeof(NV_ENC_REGISTER_RESOURCE));
	std::memset(upOutFencePoint.get(), 0, sizeof(NV_ENC_FENCE_POINT_D3D12));

	// Define fields of NV_ENC_REGISTER_RESOURCE
	upNvRegRes->version = NV_ENC_REGISTER_RESOURCE_VER;
	upNvRegRes->resourceType = NV_ENC_INPUT_RESOURCE_TYPE_DIRECTX;
	upNvRegRes->width = iWidth;
	upNvRegRes->height = iHeight;
	upNvRegRes->pitch = 0;
	upNvRegRes->subResourceIndex = 0;
	upNvRegRes->resourceToRegister = pOutputBuffer;
	upNvRegRes->bufferFormat = NV_ENC_BUFFER_FORMAT_UNDEFINED;
	upNvRegRes->bufferUsage = NV_ENC_OUTPUT_BITSTREAM;
	upNvRegRes->pInputFencePoint = upOutFencePoint.get();

	// Define fields of NV_ENC_FENCE_POINT_D3D12
	upOutFencePoint->version = NV_ENC_FENCE_POINT_D3D12_VER;
	upOutFencePoint->pFence = cpInputFence.Get();
	upOutFencePoint->waitValue = 10;
	upOutFencePoint->signalValue = 10;
	upOutFencePoint->bWait = 0;
	upOutFencePoint->bSignal = 0;

	nvStatus = upNvFuncList->nvEncRegisterResource(pEncoder, upNvRegRes.get());
	if (NVFAILED(nvStatus)) {
		// Failed to register output buffer in NVIDIA NVENC
		return -3;
	}

	// Save handle of registered output buffer
	pRegisteredOutputBuffer = upNvRegRes->registeredResource;

	return 0;
}