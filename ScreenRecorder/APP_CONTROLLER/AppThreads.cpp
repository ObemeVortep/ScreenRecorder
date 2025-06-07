#include "APP_CONTROLLER/AppController.h"

#include <atomic>

// Starts all threads and waits for their initialization
unsigned int AppController::StartThreads() {
	// Start the ScreenRecorder thread
	std::atomic<bool> isScreenRecorderInitOver = false;
	std::atomic<bool> isScreenRecorderInitSuccessful = false;
	ScreenRecorderThread = std::jthread(
		&AppController::StartRecorderThread,
		this,
		&ScreenRecorder,
		std::ref(isScreenRecorderInitOver),
		std::ref(isScreenRecorderInitSuccessful)
	);

	// Wait until the ScreenRecorder thread finishes initialization
	while (!isScreenRecorderInitOver.load()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	// 0x01 -> ScreenRecorder flag
	unsigned int uiInitedThreads = 0;

	// Set flag if ScreenRecorder was successfully initialized
	if (isScreenRecorderInitSuccessful.load()) {
		uiInitedThreads |= 0x01;
	}

	return uiInitedThreads;
}

// Initializes and starts a recorder in a separate thread
void AppController::StartRecorderThread(
	IRecorder* pRecorder,
	std::atomic<bool>& isInitOver,
	std::atomic<bool>& isInitSuccessful
) {
	bool res = (pRecorder->Initialize() == 0) ? true : false;
	isInitSuccessful.store(res);  // Store the result of initialization
	isInitOver.store(true);       // Mark that initialization is complete

	// Start the recorder thread only if initialization was successful
	if (res) {
		pRecorder->StartThread();
	}
}
