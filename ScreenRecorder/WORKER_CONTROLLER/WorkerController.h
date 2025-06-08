#ifndef WORKER_CONTROLLER_H
#define WORKER_CONTROLLER_H

#include "SCREEN_RECORDER/ScreenRecorder.h"		// Provides the ScreenRecorder implementation
#include "FRAME_HANDLER/FrameHandler.h"			// Provides the FrameHandler implementation

#include <thread>
#include <atomic>

class WorkerController {
public:
	// Constructor
	WorkerController();

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
	// Shared RecordedData and ProceededData
	QueueWrapper RecordedFrames;
	QueueWrapper ProceededFrames;

	// Recorder instances with their thread handles
	ScreenRecorder ScreenRecorder;
	std::jthread ScreenRecorderThread;

	// Handler instances with their thread handles
	FrameHandler FrameHandler;
	std::jthread FrameHandlerThread;

};

#endif // WORKER_CONTROLLER_H
