#include "ScreenRecorder.h"

#include <iostream>

// For ComPtr smart pointer wrapper
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

// Starts the screen recording thread (currently not implemented)
void ScreenRecorder::StartThread() {
	// Create a buffer matching the screen size
	std::vector<BYTE> vFrameData(iWidth * iHeight * 4);

	// Make 1FPS stream
	while (true) {
		// Wait untill we can use D11 Resource for screen capture
		spSharedDX11On12Texture2D->WaitAndUseD11();

		// Attempt to capture a single screenshot
		if (GetFrame(vFrameData)) {
			//pRecordedData->Push(vFrameData);
		}
		// After we got the frame, we must notify FrameHandler that he can work with D3D12Resource
		spSharedDX11On12Texture2D->NotifyEndD11();

		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / FRAMES_PER_SECOND));
	}

	return;
}

// Requests to stop the screen recording (currently not implemented)
void ScreenRecorder::EndRequest() {
	return;
}

// Captures a single frame and writes it into vFrameData
bool ScreenRecorder::GetFrame(std::vector<BYTE>& vFrameData) {
	HRESULT hr;

	// Attempt to acquire the next frame
	ComPtr<IDXGIResource> DxgiRes;
	DXGI_OUTDUPL_FRAME_INFO FrameInfo;
	hr = cpDeskDupl.Get()->AcquireNextFrame(0, &FrameInfo, DxgiRes.GetAddressOf());
	if (FAILED(hr)) {
		return false;
	}

	// Attempt to retrieve the GPU texture from the DXGI resource
	ComPtr<ID3D11Texture2D> GpuTex2D;
	hr = DxgiRes->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(GpuTex2D.GetAddressOf()));
	if (FAILED(hr)) {
		return false;
	}

	// Prepare a CPU-accessible texture description
	D3D11_TEXTURE2D_DESC desc;
	GpuTex2D->GetDesc(&desc);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;

	// Attempt to create the CPU texture
	ComPtr<ID3D11Texture2D> CpuTex2D;
	hr = spDirectX11Shared->cpDevice.Get()->CreateTexture2D(&desc, nullptr, CpuTex2D.GetAddressOf());
	if (FAILED(hr)) {
		return false;
	}

	// Attempt to open shared resource (wrapped ID3D12Resource)
	spDirectX11Shared->cpContext.Get()->CopyResource(SharedTex.Get(), GpuTex2D.Get());

	// Copy the GPU texture to the CPU texture
	spDirectX11Shared->cpContext.Get()->CopyResource(CpuTex2D.Get(), SharedTex.Get());

	// Map the CPU texture to access its pixel data
	D3D11_MAPPED_SUBRESOURCE MappedData;
	hr = spDirectX11Shared->cpContext.Get()->Map(CpuTex2D.Get(), 0, D3D11_MAP_READ, 0, &MappedData);
	if (FAILED(hr)) {
		return false;
	}

	// Copy the pixel data into vFrameData, row by row (accounting for RowPitch)
	BYTE* pSrc = reinterpret_cast<BYTE*>(MappedData.pData);
	BYTE* pDst = vFrameData.data();
	for (int y = 0; y < iHeight; y++) {
		std::memcpy(
			pDst + (y * iWidth * 4),
			pSrc + (y * MappedData.RowPitch),
			iWidth * 4
		);
	}

	// Unmap the texture and release the acquired frame
	spDirectX11Shared->cpContext.Get()->Unmap(CpuTex2D.Get(), 0);
	cpDeskDupl.Get()->ReleaseFrame();

	return true;
}