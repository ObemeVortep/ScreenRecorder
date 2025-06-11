#include "FrameHandler.h"

#include <string>
#include <fstream>

// Start thread, that will process the frame queue
void FrameHandler::StartThread() {
	static int i = 0;
	// Define a vector for raw and H.264 frame
	std::vector<unsigned char> vRawFrame;
	std::vector<unsigned char> vH264Frame;

	// Infinity loop of processing frames
	while (true) {

		// Waiting for BGRA frame in D3D12Resource
		spSharedDX11On12Texture2D->WaitAndUseD12();
		
		//pRecordedData->WaitFrontAndPop(vRawFrame);

		// Convert frame to H.264
		vH264Frame = H264Converter.Convert(vRawFrame);

		// Check, if vH264Frame contains valid frame
		if (!vH264Frame.empty()) {
			// Push it in vProceededQueue
			//pProcessedData->Push(vH264Frame);

			std::cout << "Write!" << std::endl;
			std::string s = "OUTPUT/frame" + std::to_string(i++);
			std::ofstream file(s.c_str(), std::ios::binary);
			file.write((char*)vH264Frame.data(), vH264Frame.size());
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