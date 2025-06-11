#include "DirectXManager.h"

// Constructor
DirectXManager::DirectXManager() {
	spDirectX12Shared = std::make_shared<DIRECTX12_SHARED>();
	spDirectX11On12Shared = std::make_shared<DIRECTX11ON12_SHARED>();
	spDirectX11Shared = std::make_shared<DIRECTX11_SHARED>();
}

// Destructor
DirectXManager::~DirectXManager() 
	{ }

// Inititalize DirectX structures and objects
int DirectXManager::Initialize() {
	/* Step 1: initialize DirectX12Shared structure */
	if (InitializeDX12Shared() != 0) {
		// Failed to initialize DirectX12Shared
		return -1;
	}

	/* Step 2: initialize DX11On12Shared structure */
	// Before calling this function we must ensure, that DX12 shared is inited!
	if (InitializeDX11On12Shared() != 0) {
		// Failed to initialize DirectX11On12Shared
		return -2;
	}

	/* Step 3: initialize DX11Shared structure */
	if (InitializeDX11Shared() != 0) {
		// Failed to initialize DirectX11Shared
		return -3;
	}

	return 0;
}


// Initialize spDirectX12Shared
int DirectXManager::InitializeDX12Shared() {
	HRESULT hr;

	// 1. Get D3D12 Device and set it into DX12Shared structure
	hr = D3D12CreateDevice(
		nullptr,
		D3D_FEATURE_LEVEL_11_1,					// we need DX11.1 features
		__uuidof(ID3D12Device),
		reinterpret_cast<void**>(spDirectX12Shared->cpDevice.GetAddressOf())
	);
	if (FAILED(hr)) {
		// D3D12CreateDevice is failed
		return -1;
	}

	// 2. Create command queue for DX12 from D3D12 Device
	// Thanks: learn.microsoft.com/en-us/windows/win32/api/d3d11on12/nf-d3d11on12-d3d11on12createdevice
	D3D12_COMMAND_QUEUE_DESC d3d12QueueDesc = {};
	d3d12QueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3d12QueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	hr = spDirectX12Shared->cpDevice.Get()->CreateCommandQueue(
		&d3d12QueueDesc,
		__uuidof(ID3D12CommandQueue),
		reinterpret_cast<void**>(spDirectX12Shared->cpCommandQueue.GetAddressOf())
	);
	if (FAILED(hr)) {
		// CreateCommandQueue is failed
		return -2;
	}

	return 0;
}

// Initialize InitializeDX11On12Shared
// Before calling this function we must ensure, that DX12 shared is inited!
int DirectXManager::InitializeDX11On12Shared() {
	HRESULT hr;

	// 1. Create DX11 Device and Context on DX12 Device and CommandQueue
	Microsoft::WRL::ComPtr<ID3D11Device> baseDevice;
	D3D_FEATURE_LEVEL fLevel = D3D_FEATURE_LEVEL_11_1;
	hr = D3D11On12CreateDevice(
		spDirectX12Shared->cpDevice.Get(),
		D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		&fLevel,
		1,
		reinterpret_cast<IUnknown**>(spDirectX12Shared->cpCommandQueue.GetAddressOf()),
		1,
		0,
		spDirectX11On12Shared->cpDevice.GetAddressOf(),
		spDirectX11On12Shared->cpContext.GetAddressOf(),
		nullptr
	);
	if (FAILED(hr)) {
		// Creating DX11 Device and Context on DX12 Device and CommandQueue is failed
		return -2;
	}

	// Expand the interface from D3D11Device to D3D11On12Device
	hr = spDirectX11On12Shared->cpDevice.As(&spDirectX11On12Shared->cpDeviceOn12);
	if (FAILED(hr)) {
		// Failed to expand the interface from D3D11Device to D3D11On12Device
		return -3;
	}
	// Expand the interface from D3D11On12Device to D3D11On12Device2
	hr = spDirectX11On12Shared->cpDeviceOn12.As(&spDirectX11On12Shared->cpDevice2On12);
	if (FAILED(hr)) {
		// Failed to expand the interface from D3D11On12Device to D3D11On12Device2
		return -3;
	}

	return 0;
}

// Initialize spDirectX11Shared
int DirectXManager::InitializeDX11Shared() {
	HRESULT hr;

	// 1. Create DX11 Device and Context
	D3D_FEATURE_LEVEL fLevel = D3D_FEATURE_LEVEL_11_1;
	hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		&fLevel,
		1,
		D3D11_SDK_VERSION,
		spDirectX11Shared->cpDevice.GetAddressOf(),
		nullptr,
		spDirectX11Shared->cpContext.GetAddressOf()
	);
	if (FAILED(hr)) {
		// Creating DX11 Device and Context is failed
		return -2;
	}

	// 2. Create DX11 Device1 from D3D11Device
	hr = spDirectX11Shared->cpDevice.As(&spDirectX11Shared->cpDevice1);
	if (FAILED(hr)) {
		// Creating DX11 Device1 from D3D11Device and Context is failed
		return -3;
	}

	return 0;
}