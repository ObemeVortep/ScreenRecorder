#ifndef VIDEO_PIPELINE_BUFFER_H
#define VIDEO_PIPELINE_BUFFER_H

#include "SharedQueue.h"

class VideoPipelineBuffer {
public:
	VideoPipelineBuffer() : pProcessedFrames(NULL) { }

	SharedQueue<std::vector<unsigned char>>* pProcessedFrames;
};

#endif // VIDEO_PIPELINE_BUFFER_H