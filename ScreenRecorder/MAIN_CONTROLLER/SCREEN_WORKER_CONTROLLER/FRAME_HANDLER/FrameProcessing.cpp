#include "FrameHandler.h"

// Start thread, that will process the frame queue
void FrameHandler::StartThread() {
	// Define a vector for encoded H.264 frame
	std::vector<unsigned char> vH264Frame;

	// Infinity loop of processing frames
	while (true) {
		// Waiting for BGRA frame in D3D12Resource
		spSharedDX11On12Texture2D->WaitAndUseD12();

		// Convert frame to H.264
		vH264Frame = H264Converter.Convert();

		// Check, if vH264Frame contains valid frame
		if (!vH264Frame.empty()) {
			// Push it in vProceededQueue
			spProcessedFrames.get()->Push(vH264Frame);
		}

		// After we processed the frame we must notify ScreenRecorder about that
		spSharedDX11On12Texture2D->NotifyEndD12();
	}

	return;
}

// End thread, that processes the frame queue
void FrameHandler::EndRequest() {
	return;
}