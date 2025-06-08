#ifndef FRAME_HANDLER_H
#define FRAME_HANDLER_H

#include "INTERFACES/IHandler.h"

#pragma comment (lib, "FRAME_HANDLER/libjpeg-turbo/turbojpeg-static.lib")   // For RGBA to JPEG convertion. Decided to use static for better optimization.

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

private:
    // Functions
    // Convert one raw frame from pRecorderData from raw to jpeg and place it into pProceededData
    void ConvertFrame(const std::vector<unsigned char>& vRawFrame, std::vector<unsigned char>& vJpegFrame);
};

#endif // FRAME_HANDLER_H