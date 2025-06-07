#include "WORKER_CONTROLLER/WorkerController.h"

#include <atomic>

unsigned int WorkerController::TryToStartThread(std::jthread& NewThread, IRecorder* pWorker) {
	// Current flag. After every call must be x2
	static unsigned int uiCurrentFlag = 0;
	if (uiCurrentFlag == 0) {
		uiCurrentFlag += 0x01;
	} 
	else {
		uiCurrentFlag *= 2;
	}
	
	// Define all atomic flags of thread initialization
	std::atomic<bool> isInitOver = false;
	std::atomic<bool> isInitSuccessfull = false;

	// Make new thread
	NewThread = std::jthread(
		&WorkerController::StartThread,
		this,
		pWorker,
		std::ref(isInitOver),
		std::ref(isInitSuccessfull)
	);

	// Wait untill this thread finish their initialization phase
	while (!isInitOver.load()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	// Return the corresponding flag if the ScreenRecorder was successfully initialized. Else return 0.
	if (isInitSuccessfull.load()) {
		return uiCurrentFlag;
	}
	else {
		return 0;
	}
}

// Launches all recorder threads and waits for their initialization to complete
unsigned int WorkerController::StartThreads() {
	// Bitmask flag: 0x01 = ScreenRecorder, 0x02 = FrameHandler initialized
	unsigned int uiInitedThreads = 0;

	// Try to start ScreenRecorder thread
	uiInitedThreads += TryToStartThread(ScreenRecorderThread, &ScreenRecorder);
	// Try to start FrameHandler thread
	uiInitedThreads += TryToStartThread(FrameHandlerThread, &FrameHandler);
	
	return uiInitedThreads;
}


// Initializes and starts a separate thread
void WorkerController::StartThread(
	IRecorder* pWorker,
	std::atomic<bool>& isInitOver,
	std::atomic<bool>& isInitSuccessful
) {
	bool res = (pWorker->Initialize() == 0) ? true : false;
	isInitSuccessful.store(res);  // Store the result of initialization
	isInitOver.store(true);       // Mark that initialization is complete

	// Start the recorder thread only if initialization was successful
	if (res) {
		pWorker->StartThread();
	}
}
