#include "FrameHandler.h"
#include <iostream>

// Start thread, that will process the frame queue
void FrameHandler::StartThread() {
	std::vector<unsigned char> vTemp(100);
	pRecordedData->WaitFrontAndPop(vTemp);

	std::cout << std::dec << vTemp.size() << std::endl;

	return;
}

// End thread, that processes the frame queue
void FrameHandler::EndRequest() {
	return;
}