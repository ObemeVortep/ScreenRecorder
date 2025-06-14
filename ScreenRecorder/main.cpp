#include "main.h"

#include <iostream>

int main() { 
	HRESULT hr;

	// Essential for accurate sleep times 
	timeBeginPeriod(1);

	// Essential for WASAPI
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr)) {
		// Failed to initialize COM in multithreaded environment
		return -1;
	}

	// It optimizes DirectX usage for ScreenRecorder and FrameHandler
	// This allows us to shoot and encode video completely without using the CPU
	DirectXManager directXManager;
	if (directXManager.Initialize() != 0) {
		// Failed to initialize DirectX11 / DirectX12
		return -2;
	}

	// mainController manages all data collection threads for video creation
	MainController mainController(directXManager.GetDirectX12Shared(), directXManager.GetDirectX11On12Shared(), directXManager.GetDirectX11Shared());

	// Initializes threads and returns videoPipelineBuffer, which is used as a data stream to create the video (and also contains important information for initialization)
	VideoPipelineBuffer videoPipelineBuffer = mainController.Initialize();
	if (videoPipelineBuffer.GetInitedControllers() == 0) {
		// Failed to initialize at least one recorder<->handler pair in mainController
		return -3;
	}

	// VideoCreator creates video from videoPipelineBuffer data in real time
	VideoCreator videoCreator(videoPipelineBuffer);
	if (videoCreator.Initialize() != 0) {
		// Failed to initialize VideoCreator
		return -4;
	}

	// Start video creation before capturing threads
	// This is done to prevent a data queue from being created until the VideoCreation thread in videoCreator is created
	videoCreator.StartVideoCreation();

	// Start all screen recording streams only after all dependencies have been initialized
	mainController.StartThreads();

	std::cout << "Threads started: " << videoPipelineBuffer.GetInitedControllers() << std::endl;

	while (true) {
		std::this_thread::sleep_for(std::chrono::seconds(1000));
	}

	// Is it really necessary?
	CoUninitialize();

	return 0;
}