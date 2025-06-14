#include "SysAudioRecorder.h"

#include <iostream>

// Start sysaudio capturing
void SysAudioRecorder::StartThread() {
	HRESULT hr;

	// Start Audio Client for loopback audio capture
	hr = cpAudioClient.Get()->Start();
	if (FAILED(hr)) {
		// May occur if the device was unplugged, etc.
		return;
	}

	// Main infinity loop, which records data every second
	while (true) {
		UINT32 packetLength = 0;
		// Check if we can read a data from audio buffer
		hr = cpAudioCaptureClient->GetNextPacketSize(&packetLength);
		if (FAILED(hr)) break;

		while (packetLength > 0) {
			std::vector<unsigned char> vCapturedSysAudio = CaptureAudio();
			if (!vCapturedSysAudio.empty()) {
				spRecordedSysAudio->Push(std::move(vCapturedSysAudio));
			}

			// Check for more data immediately
			hr = cpAudioCaptureClient->GetNextPacketSize(&packetLength);
			if (FAILED(hr)) break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(3));
	}
	hr = cpAudioClient.Get()->Stop();
}

// Capture audio via IAudioCaptureClient and return a vector with recorded audio
std::vector<unsigned char> SysAudioRecorder::CaptureAudio() {
	HRESULT hr;

	// Vars that we need for buffer creation
	BYTE* pData = NULL;
	UINT32 uiNumFrames = 0;
	DWORD dwFlags = 0;

	// Get buffer of recorded system audio
	hr = cpAudioCaptureClient.Get()->GetBuffer(
		&pData,
		&uiNumFrames,
		&dwFlags,
		NULL,
		NULL
	);
	if (FAILED(hr)) {
		// May occur if a buffer is empty or the device was unplugged, etc.
		return std::vector<unsigned char>();
	}

	// Copy data into a vector
	std::vector<unsigned char> vBuffer(pData, pData + (uiNumFrames * pWaveFormat->nBlockAlign));
	
	// Release a buffer
	cpAudioCaptureClient.Get()->ReleaseBuffer(uiNumFrames);

	// return a vector
	return vBuffer;
}

// End sysaudio capturing
void SysAudioRecorder::EndRequest() {
	// Temporary no implementation
	return;
}