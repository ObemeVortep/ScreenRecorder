#ifndef FRAME_HANDLER_H
#define FRAME_HANDLER_H

#include "INTERFACES/IHandler.h"

#include "TYPES/DirectXShared.h"

#include "CONVERTERS/JPEG_CONVERTER/JpegConverter.h"
#include "CONVERTERS/H264_CONVERTER/H264Converter.h"

#include <memory>

#define WIDTH	1920		// We will get it dynamic in future
#define HEIGHT	1080		// We will get it dynamic in future

// This class converts raw data from DDAPI to jpeg and save it into pProcessedData
class FrameHandler : public IHandler {
public:
	// Constructor
	FrameHandler(std::shared_ptr<DIRECTX12_SHARED> spDirectX12Shared, std::shared_ptr<SharedDX11On12Texture2D> spSharedDX11On12Texture2D, SharedQueue<std::vector<unsigned char>>* pProcessedData, SharedQueue<std::vector<unsigned char>>* pRecordedData);
	// Destructor
	~FrameHandler();

    // Initializes processing interfaces
    int Initialize() override;

    // Starts frame processing
    void StartThread() override;

    // Ends frame processing
    void EndRequest() override;

private:
    // Connectors between RECORDER <-> HANDLER
    // ScreenRecorder -> FrameHandler
    std::shared_ptr<SharedDX11On12Texture2D> spSharedDX11On12Texture2D;

private:
    // Variables
    // BGRA-to-JPEG converter. Must be initialized before using.
    JpegConverter JpegConverter;

    // BGRA-to-H264 converter. Must be initialized before using.
    H264Converter H264Converter;
};

#endif // FRAME_HANDLER_H