#ifndef MAIN_CONTROLLER_H
#define MAIN_CONTROLLER_H

#include "SCREEN_WORKER_CONTROLLER/ScreenWorkerController.h"

class MainController {
public:
	// Constructor
	MainController(std::shared_ptr<DIRECTX12_SHARED> spDirectX12Shared, std::shared_ptr<DIRECTX11ON12_SHARED> spDirectX11On12Shared, std::shared_ptr<DIRECTX11_SHARED> spDirectX11Shared);

	// Destructor
	~MainController();

	// Initialize MainController
	VideoPipelineBuffer Initialize();

	// Starts all worker controllers that were successfully initialized
	void StartThreads();

private:
	// Starts the worker controller
	void StartWorkerController(IWorkerController* pWorkerController);

private:
	// Internal state fields
	// This bit-field means count of internal controllers that were inited
	// 0x01: ScreenWorkerController
	uint32_t uiControllersInited;

private:
	// All worker controllers
	// ScreenWorkerController captures screen 60 times per second and encode it into H.264 frames
	ScreenWorkerController screenWorkerController;
};

#endif // MAIN_CONTROLLER_H