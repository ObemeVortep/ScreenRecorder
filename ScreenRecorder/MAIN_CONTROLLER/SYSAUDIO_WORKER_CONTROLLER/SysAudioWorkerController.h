#ifndef SYSAUDIO_WORKER_CONTROLLER_H
#define SYSAUDIO_WORKER_CONTROLLER_H

#include "INTERFACES/IWorkerController.h"

#include "SYSAUDIO_RECORDER/SysAudioRecorder.h"
#include "SYSAUDIO_HANDLER/SysAudioHandler.h"

class SysAudioWorkerController : public IWorkerController {
public:
	// Contstructor
	SysAudioWorkerController();
	// Destructor
	~SysAudioWorkerController();

	// Initialize all worker threads
	std::shared_ptr<SharedQueue<std::vector<unsigned char>>> Initialize() override;

	// Start all worker threads
	void StartThreads() override;

private:
	// Connectors between RECORDER -> HANDLER
	// SysAudioRecorder -> SysAudioHandler
	std::shared_ptr<SharedQueue<std::vector<unsigned char>>> spRecordedSysAudio;

private:
	// Connectors between HANDLER -> VIDEO_CREATOR
	// SysAudioHandler -> VideoCreator
	std::shared_ptr<SharedQueue<std::vector<unsigned char>>> spProcessedSysAudio;

private:
	// Recorder instances with their thread handles
	SysAudioRecorder sysAudioRecorder;
	std::jthread sysAudioRecorderThread;

	// Handler instances with their thread handles and processed queues
	SysAudioHandler sysAudioHandler;
	std::jthread sysAudioHandlerThread;
};

#endif // SYSAUDIO_WORKER_CONTROLLER_H