#include "WORKER_CONTROLLER/WorkerController.h"

// Constructor
WorkerController::WorkerController() 
	: RecordedFrames(), ScreenRecorder(&RecordedFrames),					// Recorders
	  ProceededFrames(), FrameHandler(&ProceededFrames, &RecordedFrames)	// Handlers
		{ }

// Destructor
WorkerController::~WorkerController() {
}
