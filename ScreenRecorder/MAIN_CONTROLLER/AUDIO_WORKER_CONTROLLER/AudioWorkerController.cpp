#include "AudioWorkerController.h"

// Contstructor
AudioWorkerController::AudioWorkerController()
	: spRecordedSysAudio(std::make_shared<AudioRecordedData<std::vector<unsigned char>>>()), sysAudioRecorder(spRecordedSysAudio, eRender),
	  spRecordedMicAudio(std::make_shared<AudioRecordedData<std::vector<unsigned char>>>()), micAudioRecorder(spRecordedMicAudio, eCapture),
		spProcessedAudio(std::make_shared<SharedQueue<std::vector<unsigned char>>>())
			{ }

// Destructor
AudioWorkerController::~AudioWorkerController()
	{ }

// Initialize all worker threads and return std::shared_ptr<SharedQueue<std::vector<unsigned char>>> for VideoPipelineBuffer
std::shared_ptr<SharedQueue<std::vector<unsigned char>>> AudioWorkerController::Initialize() {
	// RECORDERS MUST BE INITIALIZED BEFORE HANDLERS
	
	// Currently AudioWorkerController only works if all modules are successfully initialized
	// In the future, just one successfully initialized Recorder will be enough
	if ((sysAudioRecorder.Initialize() == 0) && (micAudioRecorder.Initialize() == 0)) {
		if (audioHandler.Initialize() == 0) {
			return spProcessedAudio;
		}
	}

	return NULL;
}

// Start all worker threads
void AudioWorkerController::StartThreads() {
	// HANDLER MUST BE STARTED BEFORE RECORDERS
	// 1. audioRecorder
	audioHandlerThread = std::jthread(&IWorker::StartThread, &audioHandler);

	// 2. sysAudioRecorderThread
	sysAudioRecorderThread = std::jthread(&IWorker::StartThread, &sysAudioRecorder);

	// 3. micAudioRecorderThread
	micAudioRecorderThread = std::jthread(&IWorker::StartThread, &micAudioRecorder);
}