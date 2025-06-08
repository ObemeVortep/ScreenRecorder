#include "WORKER_CONTROLLER/WorkerController.h"

// Constructor
WorkerController::WorkerController(VideoPipelineBuffer* pVideoPipelineBuffer)	
	: RecordedFrames(), ScreenRecorder(&RecordedFrames),					// Recorders
		ProcessedFrames(), FrameHandler(&ProcessedFrames, &RecordedFrames)	// Handlers
{
	pVideoPipelineBuffer->pProcessedFrames = &ProcessedFrames;		// We tell VideoCreator where to get the screen frames from
}

// Destructor
WorkerController::~WorkerController() { 

}
