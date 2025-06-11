#ifndef SHARED_DX11ON12_TEXTURE_2D
#define SHARED_DX11ON12_TEXTURE_2D

#include "TYPES/DirectXShared.h"
#include <dxgi1_2.h>

#include <iostream>

#include <condition_variable>
#include <mutex>
#include <atomic>
#include <memory>
#include <wrl/client.h>

class SharedDX11On12Texture2D {
public:
	// Constructor
	SharedDX11On12Texture2D(std::shared_ptr<DIRECTX12_SHARED> spDirectX12Shared, std::shared_ptr<DIRECTX11ON12_SHARED> spDirectX11On12Shared)
		: cpD3D11Texture2D(nullptr), cpD3D12Texture2D(nullptr), hShared(NULL), spDirectX12Shared(spDirectX12Shared), spDirectX11On12Shared(spDirectX11On12Shared),
			FrameUpdateNeedful(true), IsFrameCapturing(false), IsFrameProcessing(false)
				{ }
	// Destructor
	~SharedDX11On12Texture2D() {
		if (hShared) {
			CloseHandle(hShared);
		}
		if (cpD3D11Texture2D) {
			cpD3D11Texture2D->Release();
			cpD3D11Texture2D = nullptr;
		}
		if (cpD3D12Texture2D) {
			cpD3D12Texture2D->Release();
			cpD3D12Texture2D = nullptr;
		}
	}

public:
	// Functions for safe use in multi-thread environments
	// Wait untill we can use D11 Resource for screen capture
	// It sets flag "IsFrameCapturing" in true. After use we must call NotifyEndD11()
	inline void WaitAndUseD11() {
		// Lock our mutex
		std::unique_lock<std::mutex> lock(ResourceMutex);

		// Check, if our resources already in use
		// If yes, we wait for any condition
		ResourceCondition.wait(lock, [this]() { return FrameUpdateNeedful.load() && !IsFrameProcessing.load(); });

		// Set flag "IsFrameCapturing" = true
		IsFrameCapturing.store(true);
	}

	// Notify that we have ended to use D11 Resource
	// Also set flag "IsFrameCapturing" and "FrameUpdateNeedful" in false
	inline void NotifyEndD11() {
		// First set flags
		IsFrameCapturing.store(false);
		FrameUpdateNeedful.store(false);

		// Notify one thread that use has ended
		ResourceCondition.notify_one();
	}

	// Wait untill we can use D12 Resource for frame processing
	// It sets flag "IsFrameProcessing" and "FrameUpdateNeedful" in true. After use we must call NotifyEndD12()
	inline void WaitAndUseD12() {
		// Lock our mutex
		std::unique_lock<std::mutex> lock(ResourceMutex);

		// Check, if our resources already in use
		// If yes, we wait for any condition
		ResourceCondition.wait(lock, [this]() { return !FrameUpdateNeedful.load() && !IsFrameCapturing.load(); });

		// Set flags in true
		IsFrameProcessing.store(true);
		FrameUpdateNeedful.store(true);
	}

	// Notify that we have ended to use D12 Resource
	// Also set flag "IsFrameProcessing" in false
	inline void NotifyEndD12() {
		// First set flags
		IsFrameProcessing.store(false);

		// Notify one thread that use has ended
		ResourceCondition.notify_one();
	}

public:
	// Functions for initializing and working with DX11/DX12 Resources
	// Create shared handle for ID3D11Texture2D
	inline HRESULT CreateSharedHandle(HANDLE* pHandle) {
		HRESULT hr;

		// Transform ID3D11Resource to IDXGIResource
		Microsoft::WRL::ComPtr<IDXGIResource1> cpDxgiResource;
		hr = cpD3D11Texture2D->QueryInterface(__uuidof(IDXGIResource1), reinterpret_cast<void**>(cpDxgiResource.GetAddressOf()));
		if (FAILED(hr)) {
			// Failed to transform ID3D11Texture2D to IDXGIResource
			return -1;
		}

		// Get shared handle from IDXGIResource
		hr = cpDxgiResource->CreateSharedHandle(
			nullptr,
			DXGI_SHARED_RESOURCE_WRITE | DXGI_SHARED_RESOURCE_READ,
			L"ScreenCaptureHandle",
			&hShared
		);
		if (FAILED(hr)) {
			// Failed to get shared handle from IDXGIResource
			return -2;
		}
		*pHandle = hShared;

		return hr;
	}

	// Create ID3D11Texture2D with shared access
	inline HRESULT CreateSharedTexture2D(uint32_t uiWidth, uint32_t uiHeight) {
		HRESULT hr;

		/* Step 1: create ID3D11Texture2D */
		// If any texture exists -> release
		if (cpD3D11Texture2D) {
			cpD3D11Texture2D->Release();
		}

		// Prepare a shared-accessible texture description
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = uiWidth;
		desc.Height = uiHeight;
		desc.MipLevels = 1;
		desc.ArraySize = 1;;
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = 40;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = D3D11_RESOURCE_MISC_SHARED | D3D11_RESOURCE_MISC_SHARED_NTHANDLE;

		// Try to create ID3D11Texture2D
		hr = spDirectX11On12Shared->cpDevice->CreateTexture2D(
			&desc,
			nullptr,
			&cpD3D11Texture2D
		);
		if (FAILED(hr)) {
			// Failed to ID3D11Texture2D 
			return -1;
		}

		return 0;
	}

	// Create ID3D12Resource from ID3D11Resource
	inline HRESULT CreateD12ResourceFromD11(void** ppD3D12Resource) {
		HRESULT hr;

		// Check if cpD3D11Texture2D exists
		if (!cpD3D11Texture2D) {
			// cpD3D11Texture2D not exists
			return -1;
		}

		// Try to unwrap cpD3D11Texture2D
		hr = spDirectX11On12Shared->cpDevice2On12.Get()->UnwrapUnderlyingResource(
			cpD3D11Texture2D,
			spDirectX12Shared->cpCommandQueue.Get(),
			__uuidof(ID3D12Resource),
			reinterpret_cast<void**>(&cpD3D12Texture2D)
		);
		if (FAILED(hr)) {
			// Failed to unwrap cpD3D11Texture2D
			return -2;
		}
		*ppD3D12Resource = cpD3D12Texture2D;

		return 0;
	}

private:
	// Variables for multi-thread safety
	// It indicates that use of the resource (D11/D12) has ended
	std::condition_variable ResourceCondition;

	// It indicates that D11Resource is in use and that we capture the screen right now
	std::atomic<bool> IsFrameCapturing;	
	// It indicates that D12Resource is in use and that we processing raw frame right now
	std::atomic<bool> IsFrameProcessing;
	// Do we need current frame update?
	std::atomic<bool> FrameUpdateNeedful;

	// Mutex for blocking parallel using D11/D12 Resource
	std::mutex ResourceMutex;

private:
	// Shared Handle
	HANDLE hShared;

	// DirectX structures, that we will use till this object is alive
	std::shared_ptr<DIRECTX12_SHARED> spDirectX12Shared;
	std::shared_ptr<DIRECTX11ON12_SHARED> spDirectX11On12Shared;

private:

	// Two textures around which this wrapper is made
	ID3D12Resource* cpD3D12Texture2D;
	ID3D11Texture2D* cpD3D11Texture2D;

};

#endif // SHARED_DX11ON12_TEXTURE_2D