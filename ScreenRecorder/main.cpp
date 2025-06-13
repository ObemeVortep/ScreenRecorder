#include "main.h"

int main() {
	// It optimizes DirectX usage for ScreenRecorder and FrameHandler
	// This allows us to shoot and encode video completely without using the CPU
	DirectXManager DirectXManager;
	if (DirectXManager.Initialize() != 0) {
		// Failed to initialize DirectX11 / DirectX12
		return -1;
	}

	// WorkerController manages all data collection threads for video creation
	WorkerController WorkerController(DirectXManager.GetDirectX12Shared(), DirectXManager.GetDirectX11On12Shared(), DirectXManager.GetDirectX11Shared());

	// Initializes threads and returns VideoPipelineBuffer, which is used as a data stream to create the video (and also contains important information for initialization)
	VideoPipelineBuffer VideoPipelineBuffer = WorkerController.Initialize();
	if (VideoPipelineBuffer.GetInitedPairs() == 0) {
		// Failed to initialize at least one recorder<->handler pair in WorkerController
		return -2;
	}

	// VideoCreator creates video from VideoPipelineBuffer data in real time
	VideoCreator VideoCreator(&VideoPipelineBuffer);
	if (VideoCreator.Initialize() != 0) {
		// Failed to initialize VideoCreator
		return -3;
	}

	// Start all screen recording streams only after all dependencies have been initialized
	WorkerController.StartThreads();

	while (true) {
		std::this_thread::sleep_for(std::chrono::seconds(1000));
	}
}