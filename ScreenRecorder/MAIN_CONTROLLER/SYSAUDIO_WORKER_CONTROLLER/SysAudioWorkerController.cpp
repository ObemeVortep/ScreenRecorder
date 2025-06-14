#include "SysAudioWorkerController.h"

// Contstructor
SysAudioWorkerController::SysAudioWorkerController() 
	: spRecordedSysAudio(std::make_shared<SharedQueue<std::vector<unsigned char>>>()), spProcessedSysAudio(std::make_shared<SharedQueue<std::vector<unsigned char>>>()),
		sysAudioRecorder(spRecordedSysAudio)
			{ }

// Destructor
SysAudioWorkerController::~SysAudioWorkerController() 
	{ }

// Initialize all worker threads and return std::shared_ptr<SharedQueue<std::vector<unsigned char>>> for VideoPipelineBuffer
std::shared_ptr<SharedQueue<std::vector<unsigned char>>> SysAudioWorkerController::Initialize() {
	if (InitializeWorkers(&sysAudioRecorder, &sysAudioHandler) != 0) {
		return NULL;
	}

	return spProcessedSysAudio;
}

// Start all worker threads
void SysAudioWorkerController::StartThreads() {
	StartWorkers(&sysAudioRecorder, &sysAudioHandler, sysAudioRecorderThread, sysAudioHandlerThread);
}