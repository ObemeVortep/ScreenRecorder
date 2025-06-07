#ifndef APP_CONTROLLER_H
#define APP_CONTROLLER_H

#include "SCREEN_RECORDER/ScreenRecorder.h"		// Provides the ScreenRecorder implementation
#include "FRAME_HANDLER/FrameHandler.h"			// Provides the FrameHandler implementation

#include <thread>
#include <atomic>

class AppController {
public:
	// Constructor
	AppController();

	// Destructor
	~AppController();

	// Starts all recorder threads and waits for their initialization
	unsigned int StartThreads();

private:
	// Internal helper functions

	// Tries to start the thread
	unsigned int TryToStartThread(std::jthread& NewThread, IRecorder* pWorker);

	// Initializes and starts a recorder in a separate thread
	void StartThread(
		IRecorder* pWorker,
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

#endif // APP_CONTROLLER_H
