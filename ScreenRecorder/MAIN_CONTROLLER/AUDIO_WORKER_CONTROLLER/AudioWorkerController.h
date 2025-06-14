#ifndef SYSAUDIO_WORKER_CONTROLLER_H
#define SYSAUDIO_WORKER_CONTROLLER_H

#include "INTERFACES/IWorkerController.h"

#include "AUDIO_RECORDER/AudioRecorder.h"
#include "AUDIO_HANDLER/AudioHandler.h"

#include "TYPES/SharedQueue.h"

class AudioWorkerController : public IWorkerController {
public:
	// Contstructor
	AudioWorkerController();
	// Destructor
	~AudioWorkerController();

	// Initialize all worker threads
	std::shared_ptr<SharedQueue<std::vector<unsigned char>>> Initialize() override;

	// Start all worker threads
	void StartThreads() override;

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
	// System audio recorder and his thread
	// deviceType == eRender
	AudioRecorder sysAudioRecorder;
	std::jthread sysAudioRecorderThread;

	// Microphone recorder and his thread
	// deviceType == eCapture
	AudioRecorder micAudioRecorder;
	std::jthread micAudioRecorderThread;

	// Handler instances with their thread handles and processed queues
	AudioHandler audioHandler;
	std::jthread audioHandlerThread;
};

#endif // SYSAUDIO_WORKER_CONTROLLER_H