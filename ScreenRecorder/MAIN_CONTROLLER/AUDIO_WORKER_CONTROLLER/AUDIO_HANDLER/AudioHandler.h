#ifndef SYSAUDIO_HANDLER_H
#define SYSAUDIO_HANDLER_H

#include "INTERFACES/IWorker.h"

#include "TYPES/AudioRecordedData.h"

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
	// Connectors between RECORDER -> HANDLER
	// sysAudioRecorder -> AudioHandler
	std::shared_ptr<AudioRecordedData<std::vector<unsigned char>>> spRecordedSysAudio;
	// micAudioRecorder -> AudioHandler
	std::shared_ptr<AudioRecordedData<std::vector<unsigned char>>> spRecordedMicAudio;

private:
	// Connectors between HANDLER -> VIDEO_CREATOR
	// AudioHandler -> VideoCreator
	std::shared_ptr<SharedQueue<std::vector<unsigned char>>> spProcessedAudio;
};

#endif // SYSAUDIO_HANDLER_H