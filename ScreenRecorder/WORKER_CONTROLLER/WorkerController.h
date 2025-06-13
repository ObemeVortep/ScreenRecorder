#ifndef WORKER_CONTROLLER_H
#define WORKER_CONTROLLER_H

#include "TYPES/DirectXShared.h"
#include "TYPES/SharedDX11On12Texture2D.h"
#include "TYPES/VideoPipelineBuffer.h"

#include "SCREEN_RECORDER/ScreenRecorder.h"		// Provides the ScreenRecorder implementation
#include "FRAME_HANDLER/FrameHandler.h"			// Provides the FrameHandler implementation

#include <thread>
#include <atomic>
#include <memory>

class WorkerController {
public:
	// Constructor
	WorkerController(std::shared_ptr<DIRECTX12_SHARED> spDirectX12Shared, std::shared_ptr<DIRECTX11ON12_SHARED> spDirectX11On12Shared, std::shared_ptr<DIRECTX11_SHARED> spDirectX11Shared);

	// Destructor
	~WorkerController();

	// Initialize WorkerController
	VideoPipelineBuffer Initialize();

	// Starts all pairs that were successfully initialized
	void StartThreads();

private:
	// Start separate thread using IWorker pointer and jthread reference
	void StartThread(IWorker* pWorker, std::jthread& thread);

private:
	// Internal state fields
	// This bit-field means pairs that could be started together
	// 0x01: ScreenRecorder <-> FrameHandler
	uint32_t uiPairsInited;

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
	ScreenRecorder ScreenRecorder;
	std::jthread ScreenRecorderThread;

	// Handler instances with their thread handles and processed queues
	FrameHandler FrameHandler;
	std::jthread FrameHandlerThread;
};

#endif // WORKER_CONTROLLER_H