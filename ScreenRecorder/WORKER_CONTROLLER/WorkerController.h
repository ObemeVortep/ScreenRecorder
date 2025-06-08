#ifndef WORKER_CONTROLLER_H
#define WORKER_CONTROLLER_H

#include "SCREEN_RECORDER/ScreenRecorder.h"		// Provides the ScreenRecorder implementation
#include "FRAME_HANDLER/FrameHandler.h"			// Provides the FrameHandler implementation
#include "TYPES/VideoPipelineBuffer.h"

#include <thread>
#include <atomic>

class WorkerController {
public:
	// Constructor
	WorkerController(VideoPipelineBuffer* pVideoPipelineBuffer);

	// Destructor
	~WorkerController();

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
	// Recorder instances with their thread handles and recorded queues
	ScreenRecorder ScreenRecorder;
	std::jthread ScreenRecorderThread;
	SharedQueue<std::vector<unsigned char>> RecordedFrames;

	// Handler instances with their thread handles and processed queues
	FrameHandler FrameHandler;
	std::jthread FrameHandlerThread;
	SharedQueue<std::vector<unsigned char>> ProcessedFrames;
};

#endif // WORKER_CONTROLLER_H