#ifndef VIDEO_CREATOR_H
#define VIDEO_CREATOR_H

#include "TYPES/VideoPipelineBuffer.h"
#include <thread>

class VideoCreator {
public:
	// Constructor
	VideoCreator(VideoPipelineBuffer videoPipelineBuffer);
	// Destructor
	~VideoCreator();

	// Initialize VideoCreator and internal fields
	int Initialize();

	// Start video creation from data in videoPipelineBuffer
	void StartVideoCreation();

private:
	// Video creation thread, which collects all data from pipelane buffer and make video in Output/ dir
	void VideoCreationThread();

private:
	// Main connection between MainController and VideoCreator.
	/*	.uiInitedControllers -> pairs which were successfully initialized 
	*	ScreenRecorder + FrameHandler -> 0x01
	*/
	VideoPipelineBuffer videoPipelineBuffer;

private:
	// All threads that will be created in VideoCreator
	std::jthread videoCreationThread;
};

#endif // VIDEO_CREATOR_H