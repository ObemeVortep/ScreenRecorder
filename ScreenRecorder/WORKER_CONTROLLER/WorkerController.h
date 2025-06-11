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
	int Initialize();

	// Starts all recorder threads and waits for their initialization
	unsigned int StartThreads();

private:
	// Internal helper functions

	// Tries to start the thread
	unsigned int TryToStartThread(std::jthread& NewThread, IWorker* pWorker);

	// Initializes and starts a recorder in a separate thread
	void StartThread(
		IWorker* pWorker,
		std::atomic<bool>& isInitOver,
		std::atomic<bool>& isInitSuccessful
	);

private:
	// Connectors between RECORDER <-> HANDLER
	// ScreenRecorder -> FrameHandler
	std::shared_ptr<SharedDX11On12Texture2D> spSharedDX11On12Texture2D;
	SharedQueue<std::vector<unsigned char>> RecordedFrames;

private:
	// Recorder instances with their thread handles
	ScreenRecorder ScreenRecorder;
	std::jthread ScreenRecorderThread;

	// Handler instances with their thread handles and processed queues
	FrameHandler FrameHandler;
	std::jthread FrameHandlerThread;
	SharedQueue<std::vector<unsigned char>> ProcessedFrames;
};

#endif // WORKER_CONTROLLER_H