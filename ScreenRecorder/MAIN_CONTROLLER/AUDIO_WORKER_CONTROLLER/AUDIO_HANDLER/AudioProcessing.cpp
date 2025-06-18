#include "AudioHandler.h"

// Start sysaudio capturing
void AudioHandler::StartThread() {
	// Define vectors that contain audio data
	std::vector<unsigned char> vSysAudio, vMicAudio;
	std::vector<float> vConvertedSysAudio, vConvertedMicAudio, vMixedAudio;
	
	// Main audio processing loop:
	// 1. Wait for system and mic audio to be captured
	// 2. Convert both streams to a unified format (sample rate, channels, float)
	// 3. Mix system and mic audio into a single stream
	// 4. Encode mixed audio to AAC (TODO)
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
		// NOT IMPLEMENTED YET

		/* Step 5: transfer processed audios to VideoCreator */
		//spProcessedAudio->Push(std::move(vMixedAudio));
	}
}

// Mix 2 audio (vectors must be same size and have float format)
std::vector<float> AudioHandler::Mix2Audios(const std::vector<float>& vFirstAudio, const std::vector<float>& vSecondAudio) {
	// First check, if vFirstAudio and vSecondAudio have same size
	if (vFirstAudio.size() != vSecondAudio.size()) {
		// They haven't same size
		return std::vector<float>();
	}

	// Sum two vectors and save in vMixed
	std::vector<float> vMixed(vFirstAudio.size());
	for (int i = 0; i < vFirstAudio.size(); i++) {
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

	return vMixed;
}

// End sysaudio capturing
void AudioHandler::EndRequest() {
	// Temporary no implementation
	return;
}