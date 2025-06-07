#include "APP_CONTROLLER/AppController.h"

// Constructor
AppController::AppController() 
	: RecordedFrames(), ScreenRecorder(&RecordedFrames),					// Recorders
	  ProceededFrames(), FrameHandler(&ProceededFrames, &RecordedFrames)	// Handlers
		{ }

// Destructor
AppController::~AppController() {
}
