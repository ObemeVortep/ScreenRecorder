#ifndef AUDIO_RECORDER_H
#define AUDIO_RECORDER_H

#include "INTERFACES/IWorker.h"
#include "TYPES/AudioRecordedData.h"
#include <memory>

#include <mmdeviceapi.h>
#include <audioclient.h>

// For using ComPtr
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

class AudioRecorder : public IWorker {
public:
	// Constructor
	AudioRecorder(std::shared_ptr<AudioRecordedData<std::vector<unsigned char>>> spRecordedAudio, EDataFlow eType);
	// Destructor
	~AudioRecorder();

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
	// Type of device, which captures audio
	// eRender -> system audio
	// eCapture -> microphone
	// It changes initialization, but the whole logic is the same
	EDataFlow deviceType;

private:
	// Connectors between RECORDER -> HANDLER
	// AudioRecorder -> AudioHandler
	std::shared_ptr<AudioRecordedData<std::vector<unsigned char>>> spRecordedAudio;

private:
	// For audio capture
	// Format of audio
	WAVEFORMATEX* pWaveFormat;

	// The main device from which we record sound
	ComPtr<IMMDevice> cpDevice;

	// Audio client that manages audio session (starts and ends)
	ComPtr<IAudioClient> cpAudioClient;

	// AudioCaptureClient that captures audio via Loopback 
	ComPtr<IAudioCaptureClient> cpAudioCaptureClient;
};

#endif // AUDIO_RECORDER_H