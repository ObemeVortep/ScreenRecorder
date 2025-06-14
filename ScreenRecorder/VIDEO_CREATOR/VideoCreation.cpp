#include "VideoCreator.h"

#include <iostream>
#include <fstream>
#include <string>

void VideoCreator::StartVideoCreation() {
	videoCreationThread = std::jthread(&VideoCreator::VideoCreationThread, this);
}

void VideoCreator::VideoCreationThread() {
	// The encoded frame set will be written to this directory.
	std::string s = "Output/Encoded.h264";
	std::ofstream file(s.c_str(), std::ios::binary);

	std::vector<unsigned char> vH264Frame;
	while (true) {
		// Wait for new H.264 Frame
		videoPipelineBuffer.spProcessedFrames->WaitFrontAndPop(vH264Frame);

		// Save it into output
		file.write((char*)vH264Frame.data(), vH264Frame.size());
	}
}