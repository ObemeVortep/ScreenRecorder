#include "ScreenWorkerController.h"

// Constructor
ScreenWorkerController::ScreenWorkerController(std::shared_ptr<DIRECTX12_SHARED> spDirectX12Shared, std::shared_ptr<DIRECTX11ON12_SHARED> spDirectX11On12Shared, std::shared_ptr<DIRECTX11_SHARED> spDirectX11Shared)
	: spSharedDX11On12Texture2D(std::make_shared<SharedDX11On12Texture2D>(spDirectX12Shared, spDirectX11On12Shared)),
		spProcessedFrames(std::make_shared<SharedQueue<std::vector<unsigned char>>>()),
			screenRecorder(spDirectX11On12Shared, spDirectX11Shared, spSharedDX11On12Texture2D),
			frameHandler(spDirectX12Shared, spSharedDX11On12Texture2D, spProcessedFrames)
				{ }

// Destructor
ScreenWorkerController::~ScreenWorkerController() {

}

// Initialize all workers threads and return std::shared_ptr<SharedQueue<std::vector<unsigned char>>> for VideoPipelineBuffer
std::shared_ptr<SharedQueue<std::vector<unsigned char>>> ScreenWorkerController::Initialize() {
	if (InitializeWorkers(&screenRecorder, &frameHandler) != 0) {
		return NULL;
	}

	return spProcessedFrames;
}

// Start all worker threads
void ScreenWorkerController::StartThreads() {
	StartWorkers(&screenRecorder, &frameHandler, screenRecorderThread, frameHandlerThread);
}