#include "MAIN_CONTROLLER/MainController.h"

// Constructor
MainController::MainController(std::shared_ptr<DIRECTX12_SHARED> spDirectX12Shared, std::shared_ptr<DIRECTX11ON12_SHARED> spDirectX11On12Shared, std::shared_ptr<DIRECTX11_SHARED> spDirectX11Shared)
	: uiControllersInited(0),
		screenWorkerController(spDirectX12Shared, spDirectX11On12Shared, spDirectX11Shared),
	sysAudioWorkerController()
			{ }

// Destructor
MainController::~MainController() { 

}

// Initialize MainController
VideoPipelineBuffer MainController::Initialize() {
	// VideoPipelineBuffer, which is used as a data stream to create the video (and also contains important information for initialization)
	VideoPipelineBuffer videoPipelineBuffer;

	// ScreenWorkerController works with videoPipelineBuffer.spProcessedFrames
	videoPipelineBuffer.spProcessedFrames = screenWorkerController.Initialize();
	if (videoPipelineBuffer.spProcessedFrames) {
		// If screenWorkerController successfully inited: add 0x01 to uiControllersInited
		uiControllersInited += 0x01;
	}

	// SysAudioWorkerController works with videoPipelineBuffer.spProcessedSysAudio
	videoPipelineBuffer.spProcessedSysAudio = sysAudioWorkerController.Initialize();
	if (videoPipelineBuffer.spProcessedSysAudio) {
		// If sysAudioWorkerController successfully inited: add 0x01 to uiControllersInited
		uiControllersInited += 0x02;
	}

	videoPipelineBuffer.SetInitedControllers(uiControllersInited);
	return videoPipelineBuffer;
}

// Starts all pairs that were successfully initialized
void MainController::StartThreads() {
	// We must check bit-field uiPairsInited and the corresponding bits to understand which threads can be started
	// 0x01: screenWorkerController
	if (uiControllersInited & 0x01) {
		StartWorkerController(&screenWorkerController);
	}

	// 0x02: sysAudioWorkerController
	if (uiControllersInited & 0x02) {
		StartWorkerController(&sysAudioWorkerController);
	}
}

// Start separate thread using IWorker pointer and jthread reference
void MainController::StartWorkerController(IWorkerController* pWorkerController) {
	pWorkerController->StartThreads();
}