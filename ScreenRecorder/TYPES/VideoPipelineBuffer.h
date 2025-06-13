#ifndef VIDEO_PIPELINE_BUFFER_H
#define VIDEO_PIPELINE_BUFFER_H

#include "SharedQueue.h"
#include <memory>

class VideoPipelineBuffer {
public:
	VideoPipelineBuffer() : uiInitedPairs(0) { }

	// Set bit-field of inited pairs of Recorder-Handler threads
	inline void SetInitedPairs(uint32_t uiNewInitedPairs) {
		uiInitedPairs = uiNewInitedPairs;
	}
	// Get bit-field of inited pairs of Recorder-Handler  threads
	inline uint32_t GetInitedPairs() {
		return uiInitedPairs;
	}

public:
	std::shared_ptr<SharedQueue<std::vector<unsigned char>>> spProcessedFrames;

private:
	uint32_t uiInitedPairs;
};

#endif // VIDEO_PIPELINE_BUFFER_H