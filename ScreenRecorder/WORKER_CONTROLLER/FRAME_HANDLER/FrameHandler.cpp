#include "FrameHandler.h"

// Constructor
FrameHandler::FrameHandler(std::shared_ptr<DIRECTX12_SHARED> spDirectX12Shared, std::shared_ptr<SharedDX11On12Texture2D> spSharedDX11On12Texture2D, std::shared_ptr<SharedQueue<std::vector<unsigned char>>> spProcessedFrames)
	: spSharedDX11On12Texture2D(spSharedDX11On12Texture2D), spProcessedFrames(spProcessedFrames),
		H264Converter(spDirectX12Shared, spSharedDX11On12Texture2D)
			{ }

// Destructor
FrameHandler::~FrameHandler() 
	{ }

// Initializer
int FrameHandler::Initialize() {
	// Initialize JpegConverter
	if (JpegConverter.Initialize(WIDTH, HEIGHT) != 0) {
		// Failed to initialize JpegConverter
		return -1;
	}

	// Initialize H264Converter
	if (H264Converter.Initialize(WIDTH, HEIGHT) != 0) {
		// Failed to initialize H264Converter
		return -2;
	}

	return 0;
}