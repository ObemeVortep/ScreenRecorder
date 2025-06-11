#ifndef DIRECTX_MANAGER_H
#define DIRECTX_MANAGER_H

#include "TYPES/DirectXShared.h"

// We need this .h and library to use D3D12 functions
#include <d3d12.h>
#include <d3d11.h>
#include <d3d11on12.h>

#pragma comment (lib, "d3d12.lib")
#pragma comment (lib, "d3d11.lib")

#include <memory>

// It optimizes DirectX usage for ScreenRecorder and FrameHandler
// This allows us to shoot and encode video completely without using the CPU
// DirectX11 work under DirectX12 for all components compatibility
class DirectXManager {
public:
	// Constructor
	DirectXManager();
	// Destructor
	~DirectXManager();

	// Inititalize DirectX structures and objects
	int Initialize();

public:
	// Get Functions
	// Return current spDirectX12Shared structure
	inline std::shared_ptr<DIRECTX12_SHARED> GetDirectX12Shared() {
		return spDirectX12Shared;
	}

	// Return current spDirectX11On12Shared structure
	inline std::shared_ptr<DIRECTX11ON12_SHARED> GetDirectX11On12Shared() {
		return spDirectX11On12Shared;
	}

	// Return current spDirectX11Shared structure
	inline std::shared_ptr<DIRECTX11_SHARED> GetDirectX11Shared() {
		return spDirectX11Shared;
	}

private:
	// Initialize functions
	// Initialize spDirectX12Shared
	int InitializeDX12Shared();

	// Initialize spDirectX11Shared
	// Before calling this function we must ensure, that DX12 shared is inited!
	int InitializeDX11On12Shared();

	// Initialize spDirectX11Shared
	int InitializeDX11Shared();

private:
	// DirectX structures, that we share with other modules for optimizing performance
	std::shared_ptr<DIRECTX12_SHARED> spDirectX12Shared;
	std::shared_ptr<DIRECTX11ON12_SHARED> spDirectX11On12Shared;
	std::shared_ptr<DIRECTX11_SHARED> spDirectX11Shared;

};

#endif // DIRECTX_MANAGER_H