#include "ScreenRecorder.h"

// Constructor – initializes all member variables
ScreenRecorder::ScreenRecorder(std::shared_ptr<DIRECTX11ON12_SHARED> spDirectX11On12Shared, std::shared_ptr<DIRECTX11_SHARED> spDirectX11Shared, std::shared_ptr<SharedDX11On12Texture2D> spSharedDX11On12Texture2D, SharedQueue<std::vector<unsigned char>>* pRecordedData)
	: spDirectX11On12Shared(spDirectX11On12Shared), spDirectX11Shared(spDirectX11Shared), spSharedDX11On12Texture2D(spSharedDX11On12Texture2D),
		 iWidth(0), iHeight(0),
			IRecorder(pRecordedData)
				{ }

// Destructor
ScreenRecorder::~ScreenRecorder() 
	{ }

// Initialize shared D11On12 Texture2D 
int ScreenRecorder::InitializeSharedTex() {
	// 1. Check, if DirectX shared structures are valid
	if (!spDirectX11On12Shared || !spDirectX11Shared) {
		// One or two of DirectX shared structures are not valid
		return -1;
	}

	// 2. Create shared Dx11On12 Texture 2D
	if (FAILED(spSharedDX11On12Texture2D->CreateSharedTexture2D(WIDTH, HEIGHT))) {
		// Failed to create shared handle
		return -2;
	}

	// 3. Create and get shared handle
	HANDLE hShared = NULL;
	if (FAILED(spSharedDX11On12Texture2D->CreateSharedHandle(&hShared))) {
		// Failed to create shared handle
		return -3;
	}

	// 4. Open shared handle
	if (FAILED(spDirectX11Shared->cpDevice1.Get()->OpenSharedResource1(hShared, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(SharedTex.GetAddressOf())))) {
		// Failed to open shared resource
		return -4;
	}

	return 0;
}
// Initializes the capturing interfaces
int ScreenRecorder::Initialize() {
	HRESULT hr = S_OK;

	// Initialize shared D11On12 Texture2D 
	if (InitializeSharedTex() != 0) {
		// Failed to initialize shared D11On12 Texture2D 
		return -1;
	}

	// Get DXGI device from D3D11 device
	ComPtr<IDXGIDevice> DxgiDevice;
	hr = spDirectX11Shared->cpDevice.Get()->QueryInterface(
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
	hr = DxgiOutput1->DuplicateOutput(spDirectX11Shared->cpDevice.Get(), cpDeskDupl.GetAddressOf());
	if (FAILED(hr)) {
		// Failed to create desktop duplication
		return -8;
	}

	return 0;
}