#include "APP_CONTROLLER/AppController.h"

#include <atomic>

// Launches all recorder threads and waits for their initialization to complete
unsigned int AppController::StartThreads() {
	// Start the ScreenRecorder thread with status flags for initialization tracking
	std::atomic<bool> isScreenRecorderInitOver = false;
	std::atomic<bool> isScreenRecorderInitSuccessful = false;
	ScreenRecorderThread = std::jthread(
		&AppController::StartRecorderThread,
		this,
		&ScreenRecorder,
		std::ref(isScreenRecorderInitOver),
		std::ref(isScreenRecorderInitSuccessful)
	);

	// Wait until the ScreenRecorder thread has finished its initialization phase
	auto waitForInit = [](std::atomic<bool>& isInitOver) {
		while (!isInitOver.load()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		};
	waitForInit(isScreenRecorderInitOver);

	// Bitmask flag: 0x01 = ScreenRecorder initialized
	unsigned int uiInitedThreads = 0;

	// Set the corresponding flag if the ScreenRecorder was successfully initialized
	auto checkInitResult = [&uiInitedThreads](std::atomic<bool>& isInitSuccessful, unsigned int flag) {
		if (isInitSuccessful.load()) {
			uiInitedThreads |= flag;
		}
		};
	checkInitResult(isScreenRecorderInitSuccessful, 0x01);

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
