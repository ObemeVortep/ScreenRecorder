#include "APP_CONTROLLER/AppController.h"

// Constructor
AppController::AppController() 
	: RecordedFrames(), ScreenRecorder(&RecordedFrames)
		{ }

// Destructor
AppController::~AppController() {
}
