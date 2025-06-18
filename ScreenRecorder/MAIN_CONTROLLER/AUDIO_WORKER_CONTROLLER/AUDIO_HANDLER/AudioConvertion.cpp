#include "AudioHandler.h"

// Convert PCM to FLOAT and return vector with converted audio
inline void AudioHandler::ConvertPcmToFloat(std::vector<unsigned char>& vAudio) {
	// TEMPORARY NOT IMPLEMENTED
	return;
}

// Convert stereo track to mono track and return vector with converted audio
inline void AudioHandler::ConvertStereoToMono(std::vector<unsigned char>& vAudio) {
	// Convert and save mono track into vAudio
	float* pSamples = reinterpret_cast<float*>(vAudio.data());
	uint32_t uiSamples = vAudio.size() / (2 * sizeof(float));
	for (uint32_t i = 0; i < uiSamples; i++) {
		pSamples[i] = (pSamples[i * 2] + pSamples[(i * 2) + 1]) / 2;
	}

	// Resize vAudio after convertation
	vAudio.resize(uiSamples * sizeof(float));
}

// Resample audio to 48 kHz and return vector with new resampled audio
inline std::vector<float> AudioHandler::ResampleAudio(std::vector<unsigned char>& vAudio, WAVEFORMATEX* pFormat) {
	// Create a vector with pre-defined count of samples
	float ratio = static_cast<float>(SAMPLE_RATE) / static_cast<float>(pFormat->nSamplesPerSec);
	DWORD oldSamplesCount = vAudio.size() / sizeof(float);
	DWORD newSamplesCount = static_cast<DWORD>(oldSamplesCount * ratio);
	std::vector<float> vResampledAudio(newSamplesCount);

	// Create a input struct for resampling
	SRC_DATA srcData;
	// Describe input buffer
	srcData.data_in = reinterpret_cast<const float*>(vAudio.data());
	srcData.input_frames = oldSamplesCount;

	// Describe output buffer
	srcData.data_out = vResampledAudio.data();
	srcData.input_frames = newSamplesCount;

	// Describe resample ratio
	srcData.src_ratio = ratio;

	// Call resample function
	if (src_process(pConverter, &srcData) != 0) {
		// src_process ended with error
		return std::vector<float>();
	}

	return vResampledAudio;
}

// By default, convertation to a 48 kHz float mono track is set
std::vector<float> AudioHandler::ConvertAudioToDefault(std::vector<unsigned char>& vOrigAudio, NEEDED_CONVERTS* pNeededConverts, WAVEFORMATEX* pFormat) {
	/* Step 1: convert pcm to float, if needed */
	if (pNeededConverts->isPcmToFloatConvNeeded) {
		ConvertPcmToFloat(vOrigAudio);
	}

	/* Step 2: convert stereo to mono, if needed */
	if (pNeededConverts->isMonoConvNeeded) {
		ConvertStereoToMono(vOrigAudio);
	}

	/* Step 3: resample audio, if needed (it converts unsigned char to float automatic) */
	if (pNeededConverts->isResamplingNeeded) {
		return ResampleAudio(vOrigAudio, pFormat);
	}
	
	// Otherwise make a std::vector<float> from std::vector<unsigned char> vOrigAudio
	const float* pBegin = reinterpret_cast<const float*>(vOrigAudio.data());
	const float* pEnd = pBegin + (vOrigAudio.size() / (sizeof(float)));
	return std::vector<float>(pBegin, pEnd);
}