#ifndef AUDIO_HANDLER_H
#define AUDIO_HANDLER_H

#include "INTERFACES/IWorker.h"

#include "TYPES/AudioRecordedData.h"

// We need 48 kHz discretization of audio stream
#define SAMPLE_RATE			48000

// For resampling audio stream
#include "libsamplerate/samplerate.h"
#pragma comment(lib, "MAIN_CONTROLLER/AUDIO_WORKER_CONTROLLER/AUDIO_HANDLER/libsamplerate/samplerate.lib")

// For encode audio stream in AAC
#include "fdk-aac/aacenc_lib.h"
#pragma comment(lib, "MAIN_CONTROLLER/AUDIO_WORKER_CONTROLLER/AUDIO_HANDLER/libsamplerate/samplerate.lib")


// Struct that defines, what converts do we need
typedef struct _NEEDED_CONVERTS {
	// Does audio stream need pcm-to-float convertation?
	bool isPcmToFloatConvNeeded;
	// Does audio stream need stereo-to-mono convertation?
	bool isMonoConvNeeded;
	// Does audio stream need resampling?
	bool isResamplingNeeded;
} NEEDED_CONVERTS;

class AudioHandler : public IWorker {
public:
	// Constructor
	AudioHandler(std::shared_ptr<AudioRecordedData<std::vector<unsigned char>>> spRecordedSysAudio, std::shared_ptr<AudioRecordedData<std::vector<unsigned char>>> spRecordedMicAudio, std::shared_ptr<SharedQueue<std::vector<unsigned char>>> spProcessedAudio);
	// Destructor
	~AudioHandler();

	// Initialize AudioHandler instances
	int Initialize() override;

	// Start sysaudio capturing
	void StartThread() override;

	// End sysaudio capturing
	void EndRequest() override;

private:
	// Functions used during initialization
	// Check if audio stream needs stereo-to-mono convertation
	void IsMonoConvNeeded(uint16_t uiChannels, bool* isMonoConvNeeded);

	// Check if audio stream needs pcm-to-float convertation
	// Returns false if audio format is neither PCM nor FLOAT format
	bool IsPcmToFloatConvNeeded(WAVEFORMATEX* pFormat, bool* isPcmToFloatConvNeeded);

	// Check if audio stream needs resampling
	void IsResamplingNeeded(uint32_t uiSampleRate, bool* isResamplingNeeded);

	// Initialize sample rate converter
	int InitializeSampleConverter();
private:
	// Functions that convert an audio stream. 
	// By default, convertation to a 48 kHz float mono track is set
	std::vector<float> ConvertAudioToDefault(std::vector<unsigned char>& vOrigAudio, NEEDED_CONVERTS* pNeededConverts, WAVEFORMATEX* pFormat);

	// Convert PCM to FLOAT and return vector with converted audio
	inline void ConvertPcmToFloat(std::vector<unsigned char>& vAudio);

	// Convert stereo track to mono track and return vector with converted audio
	inline void ConvertStereoToMono(std::vector<unsigned char>& vAudio);

	// Resample audio to 48 kHz and return vector with new resampled audio
	inline std::vector<float> ResampleAudio(std::vector<unsigned char>& vAudio, WAVEFORMATEX* pFormat);

private:
	// Mix 2 audio (vectors must be same size and have float format)
	std::vector<float> Mix2Audios(const std::vector<float>& vFirstAudio, const std::vector<float>& vSecondAudio);

private:
	// Connectors between RECORDER -> HANDLER
	// sysAudioRecorder -> AudioHandler
	std::shared_ptr<AudioRecordedData<std::vector<unsigned char>>> spRecordedSysAudio;
	// micAudioRecorder -> AudioHandler
	std::shared_ptr<AudioRecordedData<std::vector<unsigned char>>> spRecordedMicAudio;

private:
	// Connectors between HANDLER -> VIDEO_CREATOR
	// AudioHandler -> VideoCreator
	std::shared_ptr<SharedQueue<std::vector<unsigned char>>> spProcessedAudio;

private:
	// Structs that defines, what converts do sys and mic audios need. By default everything is set to false.
	// For sys audio
	NEEDED_CONVERTS sysConverts;
	// For mic audio
	NEEDED_CONVERTS micConverts;

private:
	// Converter from libsamplerate, that we use for resampling audio stream to 48 kHz
	SRC_STATE* pConverter;
};

#endif // AUDIO_HANDLER_H