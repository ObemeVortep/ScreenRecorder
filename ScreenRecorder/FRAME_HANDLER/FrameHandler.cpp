#include "FrameHandler.h"

// Constructor
FrameHandler::FrameHandler(SharedQueue<std::vector<unsigned char>>* pProcessedData, SharedQueue<std::vector<unsigned char>>* pRecordedData)
	: iWidth(WIDTH), iHeight(HEIGHT),
		IHandler(pProcessedData, pRecordedData)
			{ }

// Destructor
FrameHandler::~FrameHandler() {

}

// Initializer
int FrameHandler::Initialize() {
	return 0;
}