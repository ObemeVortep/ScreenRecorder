#include "VideoCreator.h"

// Constructor
VideoCreator::VideoCreator(VideoPipelineBuffer* pVideoPipelineBuffer, unsigned int uiActiveThreads)
	: pVideoPipelineBuffer(pVideoPipelineBuffer), uiActiveThreads(uiActiveThreads)
		{ }

// Destructor
VideoCreator::~VideoCreator() {

}