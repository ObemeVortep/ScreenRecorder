#include "FrameHandler.h"

// Constructor
FrameHandler::FrameHandler(QueueWrapper* pProceededData, QueueWrapper* pRecordedData) 
	: IHandler(pProceededData, pRecordedData)
		{ }

// Destructor
FrameHandler::~FrameHandler() {

}

// Initializer
int FrameHandler::Initialize() {
	return 0;
}