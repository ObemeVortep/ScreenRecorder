#ifndef JPEG_CONVERTER_H
#define JPEG_CONVERTER_H

#include "INTERFACES/IConverter.h"

// For RGBA to JPEG convertion. Decided to use static for better optimization.
#pragma comment (lib, "FRAME_HANDLER/CONVERTERS/JPEG_CONVERTER/libjpeg-turbo/turbojpeg-static.lib")   

class JpegConverter : public IConverter {
public:
	// Constructor
	JpegConverter();
	// Destructor 
	~JpegConverter();

	// Returns 0 if successfull initialized
	int Initialize(int iNewWidth, int iNewHeight) override;

	// Convert raw BGRA frame to .jpeg format
	std::vector<unsigned char> Convert(const std::vector<unsigned char>& vRawFrame) override;

private:
	// Variables

	// Screen metrics
	int iWidth;
	int iHeight;
};

#endif // JPEG_CONVERTER_H