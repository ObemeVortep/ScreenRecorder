#include "WORKER_CONTROLLER/WorkerController.h"

// Constructor
WorkerController::WorkerController(std::shared_ptr<DIRECTX12_SHARED> spDirectX12Shared, std::shared_ptr<DIRECTX11ON12_SHARED> spDirectX11On12Shared, std::shared_ptr<DIRECTX11_SHARED> spDirectX11Shared)
	:	spSharedDX11On12Texture2D(std::make_shared<SharedDX11On12Texture2D>(spDirectX12Shared, spDirectX11On12Shared)), 
		spProcessedFrames(std::make_shared<SharedQueue<std::vector<unsigned char>>>()),
			ScreenRecorder(spDirectX11On12Shared, spDirectX11Shared, spSharedDX11On12Texture2D),					
			FrameHandler(spDirectX12Shared, spSharedDX11On12Texture2D, spProcessedFrames)
				{ }

// Destructor
WorkerController::~WorkerController() { 

}

// Initialize WorkerController
int WorkerController::Initialize() {
	return 0;
}