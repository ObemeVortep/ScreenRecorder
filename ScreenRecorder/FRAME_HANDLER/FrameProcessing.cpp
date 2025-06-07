#include "FrameHandler.h"

// Start thread, that will process the frame queue
void FrameHandler::StartThread() {
	// Define a vector for processed frames
	std::vector<unsigned char> vTemp;

	// Process first frame
	pRecordedData->WaitFrontAndPop(vTemp);

	return;
}

// End thread, that processes the frame queue
void FrameHandler::EndRequest() {
	return;
}