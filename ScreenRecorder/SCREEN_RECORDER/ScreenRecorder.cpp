#include "ScreenRecorder.h"

// For ComPtr smart pointer wrapper
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

// Constructor – initializes all member variables
ScreenRecorder::ScreenRecorder()
	: pDevice(nullptr), pDeskDupl(nullptr), iWidth(0), iHeight(0)
{
}

// Destructor – releases COM interfaces in correct order
ScreenRecorder::~ScreenRecorder() {
	// Release order: pDeskDupl -> pDevice

	if (pDeskDupl) {
		pDeskDupl->Release();
	}
	if (pDevice) {
		pDevice->Release();
	}
}

// Initializes the capturing interfaces
int ScreenRecorder::Initialize() {
	HRESULT hr = S_OK;

	// Try to create a D3D11 device
	hr = D3D11CreateDevice(
		nullptr,								// Use the default adapter
		D3D_DRIVER_TYPE_HARDWARE,				// Preferred driver type
		nullptr,								// No software device
		D3D11_CREATE_DEVICE_SINGLETHREADED,		// For better performance
		nullptr, 0,								// Default feature levels
		D3D11_SDK_VERSION,						// Must be D3D11_SDK_VERSION
		&pDevice,
		nullptr,								// We don't need the feature level
		nullptr									// We don't need the device context
	);
	if (FAILED(hr)) {
		// Failed to create D3D11 device
		return -1;
	}

	// Get DXGI device from D3D11 device
	ComPtr<IDXGIDevice> DxgiDevice;
	hr = pDevice->QueryInterface(
		__uuidof(IDXGIDevice),
		reinterpret_cast<void**>(DxgiDevice.GetAddressOf())
	);
	if (FAILED(hr)) {
		// Failed to get DXGI device
		return -2;
	}

	// Get DXGI adapter from DXGI device
	ComPtr<IDXGIAdapter> DxgiAdapter;
	hr = DxgiDevice.Get()->GetParent(
		__uuidof(IDXGIAdapter),
		reinterpret_cast<void**>(DxgiAdapter.GetAddressOf())
	);
	if (FAILED(hr)) {
		// Failed to get DXGI adapter
		return -3;
	}

	// Get the output (monitor). EnumOutputs only returns IDXGIOutput, not IDXGIOutput1
	ComPtr<IDXGIOutput> DxgiOutput;
	hr = DxgiAdapter.Get()->EnumOutputs(
		0,
		DxgiOutput.GetAddressOf()
	);
	if (FAILED(hr)) {
		// Failed to get first DXGI output
		return -4;
	}

	// Get monitor width and height
	DXGI_OUTPUT_DESC DxgiDesc = {};
	hr = DxgiOutput->GetDesc(&DxgiDesc);
	if (FAILED(hr)) {
		// Failed to get DXGI_OUTPUT_DESC
		return -5;
	}

	iWidth = DxgiDesc.DesktopCoordinates.right - DxgiDesc.DesktopCoordinates.left;
	iHeight = DxgiDesc.DesktopCoordinates.bottom - DxgiDesc.DesktopCoordinates.top;

	if (iWidth <= 0 || iHeight <= 0) {
		// Invalid monitor dimensions
		return -6;
	}

	// Query IDXGIOutput1 from IDXGIOutput
	ComPtr<IDXGIOutput1> DxgiOutput1;
	hr = DxgiOutput.Get()->QueryInterface(
		__uuidof(IDXGIOutput1),
		reinterpret_cast<void**>(DxgiOutput1.GetAddressOf())
	);
	if (FAILED(hr)) {
		// Failed to get IDXGIOutput1 interface
		return -7;
	}

	// Create the desktop duplication interface
	hr = DxgiOutput1->DuplicateOutput(pDevice, &pDeskDupl);
	if (FAILED(hr)) {
		// Failed to create desktop duplication
		return -8;
	}

	return 0;
}