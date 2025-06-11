#include "FrameHandler.h"

// Constructor
FrameHandler::FrameHandler(std::shared_ptr<DIRECTX12_SHARED> spDirectX12Shared, std::shared_ptr<SharedDX11On12Texture2D> spSharedDX11On12Texture2D, SharedQueue<std::vector<unsigned char>>* pProcessedData, SharedQueue<std::vector<unsigned char>>* pRecordedData)
	: spSharedDX11On12Texture2D(spSharedDX11On12Texture2D),
		H264Converter(spDirectX12Shared, spSharedDX11On12Texture2D),
			IHandler(pProcessedData, pRecordedData)
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