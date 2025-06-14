#include "ScreenRecorder.h"

// For time-stamps
#include <chrono>

// For ComPtr smart pointer wrapper
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

// Starts the screen recording thread (currently not implemented)
void ScreenRecorder::StartThread() {
	// Create a buffer matching the screen size
	std::vector<BYTE> vFrameData(iWidth * iHeight * 4);
	
	// We must capture a frame every 16666700 nanoseconds (60 FPS)
	constexpr auto kTargetFrameInterval = std::chrono::nanoseconds(16666700);

	// Make 1FPS stream
	while (true) {
		// Record the time we started screen capture
		auto start = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());

		// Wait untill we can use D11 Resource for screen capture
		spSharedDX11On12Texture2D->WaitAndUseD11();

		// Attempt to capture a single screenshot
		// Can return false, but we can ignore it
		GetFrame(vFrameData);

		// After we got the frame, we must notify FrameHandler that he can work with D3D12Resource
		spSharedDX11On12Texture2D->NotifyEndD11();

		// Record the time we ended screen capture
		auto end = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());

		// Calculate how long the frame took
		auto elapsed = end - start;

		// Sleep if the frame was faster than the target interval
		if (elapsed < kTargetFrameInterval - std::chrono::microseconds(2000)) {
			std::this_thread::sleep_for((kTargetFrameInterval - elapsed) - std::chrono::microseconds(1000));
		}

		// Spin for final precision
		while (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()) - start < kTargetFrameInterval) {
			std::this_thread::yield(); // or just busy wait
		}
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

	// Ñopy raw BGRA data to our wrapped D3D12Resource
	spDirectX11Shared->cpContext.Get()->CopyResource(SharedTex.Get(), GpuTex2D.Get());

	// Release frame after that
	cpDeskDupl.Get()->ReleaseFrame();

	return true;
}