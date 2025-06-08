#include "FrameHandler.h"

// Constructor
FrameHandler::FrameHandler(SharedQueue<std::vector<unsigned char>>* pProceededData, SharedQueue<std::vector<unsigned char>>* pRecordedData)
	: iWidth(WIDTH), iHeight(HEIGHT),
		IHandler(pProceededData, pRecordedData)
			{ }

// Destructor
FrameHandler::~FrameHandler() {

}

// Initializer
int FrameHandler::Initialize() {
	return 0;
}