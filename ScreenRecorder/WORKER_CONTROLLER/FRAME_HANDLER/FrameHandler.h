#ifndef FRAME_HANDLER_H
#define FRAME_HANDLER_H

#include "INTERFACES/IWorker.h"

#include "TYPES/DirectXShared.h"

#include "CONVERTERS/JPEG_CONVERTER/JpegConverter.h"
#include "CONVERTERS/H264_CONVERTER/H264Converter.h"

#include <memory>

#define WIDTH	1920		// We will get it dynamic in future
#define HEIGHT	1080		// We will get it dynamic in future

// This class converts raw data from DDAPI to jpeg and save it into pProcessedData
class FrameHandler : public IWorker {
public:
	// Constructor
	FrameHandler(std::shared_ptr<DIRECTX12_SHARED> spDirectX12Shared, std::shared_ptr<SharedDX11On12Texture2D> spSharedDX11On12Texture2D, std::shared_ptr<SharedQueue<std::vector<unsigned char>>> spProcessedFrames);
	// Destructor
	~FrameHandler();

    // Initializes processing interfaces
    int Initialize() override;

    // Starts frame processing
    void StartThread() override;

    // Ends frame processing
    void EndRequest() override;

private:
    // Connectors between RECORDER -> HANDLER
    // ScreenRecorder -> FrameHandler
    // Must be forwarded to H264Converter for optimizing!
    std::shared_ptr<SharedDX11On12Texture2D> spSharedDX11On12Texture2D;

private:
    // Connectors between HANDLER -> VIDEO_CREATOR
    // FrameHandler -> VideoCreator
    std::shared_ptr<SharedQueue<std::vector<unsigned char>>> spProcessedFrames;

private:
    // Variables
    // BGRA-to-JPEG converter. Must be initialized before using.
    JpegConverter JpegConverter;

    // BGRA-to-H264 converter. Must be initialized before using.
    H264Converter H264Converter;
};

#endif // FRAME_HANDLER_H