#include "WORKER_CONTROLLER/WorkerController.h"

// Starts all pairs that were successfully initialized
void WorkerController::StartThreads() {
	// We must check bit-field uiPairsInited and the corresponding bits to understand which threads can be started
	// HANDLERS MUST BE STARTED BEFORE RECORDERS
	// 0x01: ScreenRecorder <-> FrameHandler
	if (uiPairsInited & 0x01) {
		StartThread(&FrameHandler, FrameHandlerThread);
		StartThread(&ScreenRecorder, ScreenRecorderThread);
	}
}

// Start separate thread using IWorker pointer and jthread reference
void WorkerController::StartThread(IWorker* pWorker, std::jthread& thread) {
	thread = std::jthread(&IWorker::StartThread, pWorker);
}