#include "main.h"

#include <iostream>

int main() {
	timeBeginPeriod(1);

	// It optimizes DirectX usage for ScreenRecorder and FrameHandler
	// This allows us to shoot and encode video completely without using the CPU
	DirectXManager directXManager;
	if (directXManager.Initialize() != 0) {
		// Failed to initialize DirectX11 / DirectX12
		return -1;
	}

	// workerController manages all data collection threads for video creation
	WorkerController workerController(directXManager.GetDirectX12Shared(), directXManager.GetDirectX11On12Shared(), directXManager.GetDirectX11Shared());

	// Initializes threads and returns videoPipelineBuffer, which is used as a data stream to create the video (and also contains important information for initialization)
	VideoPipelineBuffer videoPipelineBuffer = workerController.Initialize();
	if (videoPipelineBuffer.GetInitedPairs() == 0) {
		// Failed to initialize at least one recorder<->handler pair in workerController
		return -2;
	}

	// VideoCreator creates video from videoPipelineBuffer data in real time
	VideoCreator videoCreator(videoPipelineBuffer);
	if (videoCreator.Initialize() != 0) {
		// Failed to initialize VideoCreator
		return -3;
	}

	// Start video creation before capturing threads
	// This is done to prevent a data queue from being created until the VideoCreation thread in videoCreator is created
	videoCreator.StartVideoCreation();

	// Start all screen recording streams only after all dependencies have been initialized
	workerController.StartThreads();

	std::cout << "Threads started" << std::endl;

	while (true) {
		std::this_thread::sleep_for(std::chrono::seconds(1000));
	}
}