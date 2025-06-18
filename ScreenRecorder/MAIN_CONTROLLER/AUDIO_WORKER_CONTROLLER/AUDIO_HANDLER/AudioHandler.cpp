#include "AudioHandler.h"

// Constructor implementation
AudioHandler::AudioHandler(std::shared_ptr<AudioRecordedData<std::vector<unsigned char>>> spRecordedSysAudio, std::shared_ptr<AudioRecordedData<std::vector<unsigned char>>> spRecordedMicAudio, std::shared_ptr<SharedQueue<std::vector<unsigned char>>> spProcessedAudio)
	: spRecordedSysAudio(spRecordedSysAudio), spRecordedMicAudio(spRecordedMicAudio), spProcessedAudio(spProcessedAudio),
		pSampleRateConverter(nullptr), hAacEncoder(nullptr)
{
	// Set all fields in sysConverts to false
	sysConverts.isMonoConvNeeded = false;
	sysConverts.isPcmToFloatConvNeeded = false;
	sysConverts.isResamplingNeeded = false;

	// Set all fields in micConverts to false
	micConverts.isMonoConvNeeded = false;
	micConverts.isPcmToFloatConvNeeded = false;
	micConverts.isResamplingNeeded = false;
}

// Destructor implementation
AudioHandler::~AudioHandler() {
	if (pSampleRateConverter) {
		src_delete(pSampleRateConverter);
	}
	if (hAacEncoder) {
		aacEncClose(&hAacEncoder);
	}
}

// Check if audio stream needs pcm-to-float convertation
// Returns false if audio format is neither PCM nor FLOAT format
bool AudioHandler::IsPcmToFloatConvNeeded(WAVEFORMATEX* pFormat, bool* isFloatConvNeeded) {
	if (pFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
		// We have WAVEFORMATEXTENSIBLE
		if (reinterpret_cast<WAVEFORMATEXTENSIBLE*>(pFormat)->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT) {
			// We have already float format, no convertation is needed
			*isFloatConvNeeded = false;
			return true;
		}
		else if (reinterpret_cast<WAVEFORMATEXTENSIBLE*>(pFormat)->SubFormat == KSDATAFORMAT_SUBTYPE_ADPCM) {
			// We have PCM format, pcm-to-float convertation is needed
			*isFloatConvNeeded = true;
			return true;
		}
	}
	else {
		// We have default WAVEFORMATEX and can check wFormatTag == WAVE_FORMAT_IEEE_FLOAT
		if (pFormat->wFormatTag == WAVE_FORMAT_IEEE_FLOAT) {
			// We have already float format, no convertation is needed
			*isFloatConvNeeded = false;
			return true;
		}
		else if (pFormat->wFormatTag == WAVE_FORMAT_PCM) {
			// We have PCM format, pcm-to-float convertation is needed
			*isFloatConvNeeded = true;
			return true;
		}
	}

	// Format is neither PCM nor FLOAT
	return false;
}

// Check if audio stream needs stereo-to-mono convertation
void AudioHandler::IsMonoConvNeeded(uint16_t uiChannels, bool* isMonoConvNeeded) {
	if (uiChannels != 1) {
		// Stereo-to-mono convertation for audio is needed
		*isMonoConvNeeded = true;
	}
	else {
		// Stereo-to-mono convertation for audio is not needed
		*isMonoConvNeeded = false;
	}
}

// Check if audio stream needs resampling
void AudioHandler::IsResamplingNeeded(uint32_t uiSampleRate, bool* isResamplingNeeded) {
	if (uiSampleRate != SAMPLE_RATE) {
		// Resampling for audio is needed
		*isResamplingNeeded = true;
	}
	else {
		// Resampling for audio is not needed
		*isResamplingNeeded = false;
	}
}

// Initialize sample rate converter
int AudioHandler::InitializeSampleRateConverter() {
	/* Step 1: create a new sample rate converter */
	INT err = 0;
	pSampleRateConverter = src_new(SRC_SINC_FASTEST, 1, &err);
	if (!pSampleRateConverter) {
		return -1;
	}

	return 0;
}

// Initialize AAC encoder
int AudioHandler::InitializeAACEncoder() {
	AACENC_ERROR err;

	/* Step 1: allocate AAC encoder with required configuration */
	err = aacEncOpen(
		&hAacEncoder, 
		0x01,			// Allocate only AAC Core Module
		1				// We have a single-channel audio stream.
	);
	if (err != AACENC_OK) {
		// Failed to allocate AAC encoder
		return -1;
	}

	/* Step 2: set encoder parameters */
	// 1. Set AOT - MPEG-4 AAC Low-Delay
	err = aacEncoder_SetParam(hAacEncoder, AACENC_AOT, 23);
	if (err != AACENC_OK) {
		// Failet to set AOT
		return -2;
	}
	// 2. Set bitrate - 64 Kb per second
	err = aacEncoder_SetParam(hAacEncoder, AACENC_BITRATE, 64000);
	if (err != AACENC_OK) {
		// Failed to set bitrate
		return -2;
	}
	// 3. Set sample rate - pre-defined SAMPLE_RATE
	err = aacEncoder_SetParam(hAacEncoder, AACENC_SAMPLERATE, SAMPLE_RATE);
	if (err != AACENC_OK) {
		// Failed to set sample rate
		return -2;
	}
	// 4. Set channel mode - we set 1, because we have only one
	err = aacEncoder_SetParam(hAacEncoder, AACENC_CHANNELMODE, 1);
	if (err != AACENC_OK) {
		// Failed to set channel mode
		return -2;
	}

	/* Step 3: initialize encoder with setted parameters */
	// Call the function with NULL parameters to initialize the encoder
	err = aacEncEncode(hAacEncoder, NULL, NULL, NULL, NULL);
	if (err != AACENC_OK) {
		// Failed to initialize AAC encoder
		return -3;
	}

	return 0;
}

// Initialize AudioHandler instances
int AudioHandler::Initialize() {
	// We can without fear use AudioRecordedData, because they must be inited in Recorder's instances before Handler's initializer was called

	/* Step 1: check, which audio streams need pcm-to-float convertation */
	// 1. Check if sys audio needs pcm-to-float convertation.
	if (!IsPcmToFloatConvNeeded(spRecordedMicAudio->GetAudioFormat(), &sysConverts.isPcmToFloatConvNeeded)) {
		return -1;
	}
	// 2. Check if mic audio needs pcm-to-float convertation.
	if (!IsPcmToFloatConvNeeded(spRecordedMicAudio->GetAudioFormat(), &micConverts.isPcmToFloatConvNeeded)) {
		return -1;
	}

	/* Step 2: check, which audio streams need stereo-to-mono convertation */
	// 1. Check if sys audio needs stereo-to-mono convertation.
	IsMonoConvNeeded(spRecordedSysAudio->GetAudioFormat()->nChannels, &sysConverts.isMonoConvNeeded);
	// 2. Check if mic audio needs stereo-to-mono convertation.
	IsMonoConvNeeded(spRecordedMicAudio->GetAudioFormat()->nChannels, &micConverts.isMonoConvNeeded);

	/* Step 3: check, which audio streams need resampling */
	// 1. Check if sys audio needs resampling.
	IsResamplingNeeded(spRecordedSysAudio->GetAudioFormat()->nSamplesPerSec, &sysConverts.isResamplingNeeded);
	// 2. Check if mic audio stream needs resampling.
	IsResamplingNeeded(spRecordedMicAudio->GetAudioFormat()->nSamplesPerSec, &micConverts.isResamplingNeeded);

	/* Step 4: initialize sample rate converter for 48 kHz resampling */
	if (InitializeSampleRateConverter() != 0) {
		// Failed to initialize sample rate converter
		return -2;
	}

	/* Step 5: initialize AAC encoder */
	if (InitializeAACEncoder() != 0) {
		// Failed to initialize AAC encoder
		return -3;
	}

	return 0; 
}