#ifndef FRAME_HANDLER_H
#define FRAME_HANDLER_H

#include "INTERFACES/IHandler.h"

// This class converts raw data from DDAPI to jpeg and save it into pProceededData
class FrameHandler : public IHandler {
public:
	// Constructor
	FrameHandler(QueueWrapper* pProceededData, QueueWrapper* pRecordedData);
	// Destructor
	~FrameHandler();

    // Initializes processing interfaces
    int Initialize() override;

    // Starts frame processing
    void StartThread() override;

    // Ends frame processing
    void EndRequest() override;
};

#endif // FRAME_HANDLER_H