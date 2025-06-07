#ifndef APP_CONTROLLER_H
#define APP_CONTROLLER_H

#include "SCREEN_RECORDER/ScreenRecorder.h"  // Provides the ScreenRecorder implementation

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

	// Initializes and starts a recorder in a separate thread
	void StartRecorderThread(
		IRecorder* pRecorder,
		std::atomic<bool>& isInitOver,
		std::atomic<bool>& isInitSuccessful
	);

private:
	// Recorder instances and their thread handles
	ScreenRecorder ScreenRecorder;
	std::jthread ScreenRecorderThread;

};

#endif // APP_CONTROLLER_H
