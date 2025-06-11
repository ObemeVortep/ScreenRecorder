#include "main.h"

#include <iostream>

int main() {
	// It optimizes DirectX usage for ScreenRecorder and FrameHandler
	// This allows us to shoot and encode video completely without using the CPU
	DirectXManager DirectXManager;
	if (DirectXManager.Initialize() != 0) {
		// Failed to initialize DirectX11 / DirectX12
		return -1;
	}
	
	// Main connection between WorkerController and VideoCreator
	VideoPipelineBuffer VideoPipelineBuffer;

	// WorkerController manages all data collection threads for video creation
	WorkerController WorkerController(DirectXManager.GetDirectX12Shared(), DirectXManager.GetDirectX11On12Shared(), DirectXManager.GetDirectX11Shared());

	// Initializes threads and returns pre-defined value, that means which thread could be started
	unsigned int uiInitedThreads = WorkerController.Initialize();
	if (uiInitedThreads == 0) {
		// Failed to initialize at least one recorder<->handler pair in WorkerController
		return -2;
	}
	// Starts threads that were successfully initialized earlier.
	WorkerController.StartThreads();

	std::cout << "uiInitedThreads: " << uiInitedThreads << std::endl;

	// VideoCreator creates video from VideoPipelineBuffer data in real time
	VideoCreator VideoCreator(&VideoPipelineBuffer, uiInitedThreads);
	VideoCreator.StartVideoCreation();

	while (true) {
		std::this_thread::sleep_for(std::chrono::seconds(1000));
	}
}