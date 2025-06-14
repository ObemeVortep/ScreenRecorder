#include "JpegConverter.h"
#include "libjpeg-turbo/jpeglib.h"		// For using jpeglib-turbo functions

// Constructor
JpegConverter::JpegConverter()
	: iWidth(0), iHeight(0)
		{ }

// Destructor
JpegConverter::~JpegConverter()
	{ }

// Initializer, returns 0 if successfull initialized
int JpegConverter::Initialize(int iNewWidth, int iNewHeight) {
	// Check if metrics are valid
	if (iNewWidth <= 0 || iNewHeight <= 0) {
		// One or two metrics are invalid
		return -1;
	}
	iWidth = iNewWidth;
	iHeight = iNewHeight;

	return 0;
}

// Converts raw BGRA frame to .jpeg format. Returns empty vector if error is occured
std::vector<unsigned char> JpegConverter::Convert() {
	// We need to check vRawFrame first
	if (vRawFrame.empty()) {
		return std::vector<unsigned char>();
	}

	// Took from: github.com/libjpeg-turbo/libjpeg-turbo/blob/main/src/example.c
	// Vector that contains .jpeg frame
	std::vector<unsigned char> vJpegFrame;	

	// This struct contains the JPEG compression parameters and pointers to working space.
	struct jpeg_compress_struct cinfo;

	// This struct represents a JPEG error handler
	struct jpeg_error_mgr		jerr;

	// Another stuff
	JSAMPROW       pRow[1];	 /* for jpeg_write_scanlines() */
	unsigned char* pJpegBuffer = NULL;	/* can be optimized for std::vector in future */
	unsigned long  szBuffer = 0;

	/* Step 1: allocate and initialize JPEG compression object */
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	/* Step 2: specify data destination */
	// It allocates buffer with malloc()! We must free buf ourselves after compression!
	jpeg_mem_dest(&cinfo, &pJpegBuffer, &szBuffer);	// can be optimized for std::vector in future

	/* Step 3: set parameters for compression */
	cinfo.image_width = iWidth;
	cinfo.image_height = iHeight;
	cinfo.input_components = 4;
	cinfo.in_color_space = JCS_EXT_BGRA;	/* we need at list this before calling jpeg_set_defaults() */
	cinfo.data_precision = 8;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, 70, TRUE);

	/* Step 4: start compressor */
	jpeg_start_compress(&cinfo, TRUE);

	/* Step 5: while (scan lines ramin to be written) */
	/*			jpeg_write_scanlines(...)			  */
	while (cinfo.next_scanline < cinfo.image_height) {
		pRow[0] = (JSAMPROW)vRawFrame.data() + (cinfo.next_scanline * iWidth * 4);
		jpeg_write_scanlines(&cinfo, pRow, 1);
	}

	/* Step 6: finish compression */
	jpeg_finish_compress(&cinfo);

	/* Step 7: release compression object */
	jpeg_destroy_compress(&cinfo);

	/* Step 8: realocate pJpegBuffer into vJpegFrame */
	vJpegFrame = std::move(std::vector<unsigned char>(pJpegBuffer, pJpegBuffer + szBuffer));
	free(pJpegBuffer);	// Release using free()

	return vJpegFrame;
}