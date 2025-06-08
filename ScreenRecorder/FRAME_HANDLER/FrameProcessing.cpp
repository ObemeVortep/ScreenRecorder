#include "FrameHandler.h"

#include "libjpeg-turbo/jpeglib.h"		// For using jpeglib-turbo functions

// Start thread, that will process the frame queue
void FrameHandler::StartThread() {
	// Define a vector for raw and jpeg frames
	std::vector<unsigned char> vRawFrame;
	std::vector<unsigned char> vJpegFrame;

	// Infinity loop of processing frames
	// for debugging
	int i = 0;
	while (true) {
		// Wait for new raw frame
		pRecordedData->WaitFrontAndPop(vRawFrame);

		// Convert frame to JPEG
		ConvertFrame(vRawFrame, vJpegFrame);

		// Push it in vProceededQueue
		pProceededData->Push(vJpegFrame);

		i++;
	}

	return;
}

// Convert one raw frame from pRecorderData from raw to jpeg and place it into pProceededData
void FrameHandler::ConvertFrame(const std::vector<unsigned char>& vRawFrame, std::vector<unsigned char>& vJpegFrame) {
	// Took from: github.com/libjpeg-turbo/libjpeg-turbo/blob/main/src/example.c
	
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
		pRow[0] = (JSAMPROW)vRawFrame.data() + (cinfo.next_scanline * WIDTH * 4);
		jpeg_write_scanlines(&cinfo, pRow, 1);
	}

	/* Step 6: finish compression */
	jpeg_finish_compress(&cinfo);

	/* Step 7: release compression object */
	jpeg_destroy_compress(&cinfo);

	/* Step 8: realocate pJpegBuffer into vJpegFrame */
	vJpegFrame = std::move(std::vector<unsigned char>(pJpegBuffer, pJpegBuffer + szBuffer));
	free(pJpegBuffer);	// Release using free()

	return;
}

// End thread, that processes the frame queue
void FrameHandler::EndRequest() {
	return;
}