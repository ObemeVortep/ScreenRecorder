#ifndef SYSAUDIO_RECORDER_H
#define SYSAUDIO_RECORDER_H

#include "INTERFACES/IWorker.h"

#include "TYPES/SharedQueue.h"
#include <memory>

#include <mmdeviceapi.h>
#include <audioclient.h>

// For using ComPtr
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

class SysAudioRecorder : public IWorker {
public:
	// Constructor
	SysAudioRecorder(std::shared_ptr<SharedQueue<std::vector<unsigned char>>> spRecordedSysAudio);
	// Destructor
	~SysAudioRecorder();

	// Initialize SysAudioRecorder instances
	int Initialize() override;

	// Start sysaudio capturing
	void StartThread() override;

	// End sysaudio capturing
	void EndRequest() override;

private:
	// Capture audio via IAudioCaptureClient and return a vector with recorded audio
	std::vector<unsigned char> CaptureAudio();

private:
	// Connectors between RECORDER -> HANDLER
	// SysAudioRecorder -> SysAudioHandler
	std::shared_ptr<SharedQueue<std::vector<unsigned char>>> spRecordedSysAudio;

private:
	// For system audio capture
	// Format of system audio
	WAVEFORMATEX* pWaveFormat;

	// The main device from which we record system sound
	ComPtr<IMMDevice> cpRenderDevice;

	// Audio client that manages audio session (starts and ends)
	ComPtr<IAudioClient> cpAudioClient;

	// AudioCaptureClient that captures system audio via Loopback 
	ComPtr<IAudioCaptureClient> cpAudioCaptureClient;
};


#endif // SYSAUDIO_RECORDER_H