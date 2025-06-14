#ifndef VIDEO_PIPELINE_BUFFER_H
#define VIDEO_PIPELINE_BUFFER_H

#include "SharedQueue.h"
#include <memory>

class VideoPipelineBuffer {
public:
	VideoPipelineBuffer() : uiInitedControllers(0) { }

	// Set bit-field of inited pairs of Recorder-Handler threads
	inline void SetInitedControllers(uint32_t uiNewInitedPairs) {
		uiInitedControllers = uiNewInitedPairs;
	}
	// Get bit-field of inited pairs of Recorder-Handler  threads
	inline uint32_t GetInitedControllers() {
		return uiInitedControllers;
	}

public:
	// ScreenWorkerController works with videoPipelineBuffer.spProcessedFrames
	std::shared_ptr<SharedQueue<std::vector<unsigned char>>> spProcessedFrames;

private:
	uint32_t uiInitedControllers;
};

#endif // VIDEO_PIPELINE_BUFFER_H