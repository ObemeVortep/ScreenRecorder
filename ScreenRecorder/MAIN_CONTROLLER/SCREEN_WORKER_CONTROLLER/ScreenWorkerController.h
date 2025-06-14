#ifndef _SCREEN_WORKER_CONTROLLER_H
#define _SCREEN_WORKER_CONTROLLER_H

#include "INTERFACES/IWorkerController.h"

#include "TYPES/DirectXShared.h"
#include "TYPES/SharedDX11On12Texture2D.h"
#include "TYPES/VideoPipelineBuffer.h"

#include "SCREEN_RECORDER/ScreenRecorder.h"		// Provides the ScreenRecorder implementation
#include "FRAME_HANDLER/FrameHandler.h"			// Provides the FrameHandler implementation

class ScreenWorkerController : public IWorkerController {
public:
	// Constructor
	ScreenWorkerController(std::shared_ptr<DIRECTX12_SHARED> spDirectX12Shared, std::shared_ptr<DIRECTX11ON12_SHARED> spDirectX11On12Shared, std::shared_ptr<DIRECTX11_SHARED> spDirectX11Shared);
	// Destructor
	~ScreenWorkerController();

	// Initialize all screen worker threads
	std::shared_ptr<SharedQueue<std::vector<unsigned char>>> Initialize() override;

	// Start all screen worker threads
	void StartThreads() override;

private:
	// Connectors between RECORDER -> HANDLER
	// ScreenRecorder -> FrameHandler
	std::shared_ptr<SharedDX11On12Texture2D> spSharedDX11On12Texture2D;

private:
	// Connectors between HANDLER -> VIDEO_CREATOR
	// FrameHandler -> VideoCreator
	std::shared_ptr<SharedQueue<std::vector<unsigned char>>> spProcessedFrames;

private:
	// Recorder instances with their thread handles
	ScreenRecorder screenRecorder;
	std::jthread screenRecorderThread;

	// Handler instances with their thread handles and processed queues
	FrameHandler frameHandler;
	std::jthread frameHandlerThread;
};

#endif // _SCREEN_WORKER_CONTROLLER_H