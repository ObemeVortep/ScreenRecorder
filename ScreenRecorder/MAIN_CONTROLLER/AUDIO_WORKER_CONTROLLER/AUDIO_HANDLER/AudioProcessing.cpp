#include "AudioHandler.h"

// Start sysaudio capturing
void AudioHandler::StartThread() {
	// Define vectors that contain audio data
	std::vector<unsigned char> vSysAudio, vMicAudio, vEncodedAudio;
	std::vector<float> vConvertedSysAudio, vConvertedMicAudio;
	std::vector<INT16> vMixedAudio;
	
	// Main audio processing loop:
	// 1. Wait for system and mic audio to be captured
	// 2. Convert both streams to a unified format (sample rate, channels, float)
	// 3. Mix system and mic audio into a single stream
	// 4. Encode mixed audio to AAC
	// 5. Push final audio to VideoCreator for muxing
	while (true) {
		/* Step 1: wait for sys and mic audio to be captured */
		// FOR THE FUTURE, ADD THAT IF ONE OF THE TWO SOUNDS IS NOT AVAILABLE, IT IS NECESSARY TO USE A ZERO VECTOR INSTEAD
		spRecordedSysAudio->WaitFrontAndPop(vSysAudio);
		spRecordedMicAudio->WaitFrontAndPop(vMicAudio);

		/* Step 2: convert sys and mic audio to defaults */
		vConvertedSysAudio = ConvertAudioToDefault(vSysAudio, &sysConverts, spRecordedSysAudio->GetAudioFormat());
		vConvertedMicAudio = ConvertAudioToDefault(vMicAudio, &micConverts, spRecordedMicAudio->GetAudioFormat());

		/* Step 3: mix sys audio and mic audio */
		vMixedAudio = Mix2Audios(vConvertedSysAudio, vConvertedMicAudio);

		/* Step 4: encode mixed audio to AAC format */
		vEncodedAudio = EncodeToAAC(vMixedAudio);

		/* Step 5: transfer processed audios to VideoCreator */
		spProcessedAudio->Push(std::move(vEncodedAudio));
	}
}

// Mix 2 audio (vectors must be same size and have float format) and make from them 16bit PCM format
std::vector<INT16> AudioHandler::Mix2Audios(const std::vector<float>& vFirstAudio, const std::vector<float>& vSecondAudio) {
	// First check, if vFirstAudio and vSecondAudio have same size
	if (vFirstAudio.size() != vSecondAudio.size()) {
		// They haven't same size
		return std::vector<INT16>();
	}

	// Sum two vectors and save in vMixed
	std::vector<float> vMixed(vFirstAudio.size());
	for (size_t i = 0; i < vFirstAudio.size(); i++) {
		vMixed[i] = vFirstAudio[i] + vSecondAudio[i];
	}

	// Normalize vMixed
	float maxVal = 0.0f;
	for (float v : vMixed) {
		float absV = std::abs(v);
		maxVal = maxVal > absV ? maxVal : absV;
	}
	if (maxVal > 1.0f) {
		float scale = 1.0f / maxVal;
		for (float& v : vMixed) {
			v *= scale;
		}
	}

	// Transform FLOAT audio to PCM
	std::vector<INT16> vPcmAudio(vMixed.size());
	for (size_t i = 0; i < vMixed.size(); i++) {
		vPcmAudio[i] = vMixed[i] * 32767;
	}

	return vPcmAudio;
}

// Encode 16bit PCM audio stream in AAC format
std::vector<unsigned char> AudioHandler::EncodeToAAC(std::vector<INT16>& vAudio) {
	// Out buffer
	std::vector<unsigned char> vEncodedAudio(vAudio.size());

	/* Step 1: define input buffer description and input arguments */
	// 1. Define fields for input buffer description
	INT16* pInBuffer = vAudio.data();
	INT inBufId = IN_AUDIO_DATA;
	INT inBufSize = vAudio.size() * sizeof(INT16);
	INT inElSize = sizeof(INT16);

	// 2. Set fields of input buffer description
	AACENC_BufDesc inBufDesc;
	inBufDesc.numBufs = 1;
	inBufDesc.bufs = reinterpret_cast<void**>(&pInBuffer);
	inBufDesc.bufferIdentifiers = &inBufId;
	inBufDesc.bufSizes = &inBufSize;
	inBufDesc.bufElSizes = &inElSize;

	// 3. Define input arguments
	AACENC_InArgs inArgs;
	inArgs.numInSamples = vAudio.size();
	inArgs.numAncBytes = 0;

	/* Step 2: define output buffer description and output arguments */
	// 1. Define fields for output buffer description
	UCHAR* pOutBuffer = vEncodedAudio.data();
	INT outBufId = OUT_BITSTREAM_DATA;
	INT outBufSize = vEncodedAudio.size();
	INT outElSize = sizeof(UCHAR);

	// 2. Set fields of output buffer description
	AACENC_BufDesc outBufDesc;
	outBufDesc.numBufs = 1;
	outBufDesc.bufs = reinterpret_cast<void**>(&pOutBuffer);
	outBufDesc.bufferIdentifiers = &outBufId;
	outBufDesc.bufSizes = &outBufSize;
	outBufDesc.bufElSizes = &outElSize;

	// 3. Declare output arguments
	AACENC_OutArgs outArgs = { 0 };

	/* Step 3: call encode function */
	AACENC_ERROR err;
	err = aacEncEncode(hAacEncoder, &inBufDesc, &outBufDesc, &inArgs, &outArgs);
	if (err != AACENC_OK) {
		// Failed to encode data
		return std::vector<unsigned char>();
	}

	/* Step 4: resize and return output vector */
	vEncodedAudio.resize(outArgs.numOutBytes);

	return vEncodedAudio;
}

// End sysaudio capturing
void AudioHandler::EndRequest() {
	// Temporary no implementation
	return;
}