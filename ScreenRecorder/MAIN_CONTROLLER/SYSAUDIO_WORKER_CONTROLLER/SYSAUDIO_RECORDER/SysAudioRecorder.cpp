#include "SysAudioRecorder.h"

// Constructor implementation
SysAudioRecorder::SysAudioRecorder(std::shared_ptr<SharedQueue<std::vector<unsigned char>>> spRecordedSysAudio) 
	: spRecordedSysAudio(spRecordedSysAudio), 
		pWaveFormat(NULL)
			{ }

// Destructor implementation
SysAudioRecorder::~SysAudioRecorder() {
	if (pWaveFormat) {
		// pWaveFormat must be cleaned using CoTaskMemFree()
		CoTaskMemFree(pWaveFormat);
	}
}

// Initialize SysAudioRecorder instances
int SysAudioRecorder::Initialize() {
	HRESULT hr;

	/* Step 1: get IMMDevice Enumerator */
	ComPtr<IMMDeviceEnumerator> cpDeviceEnumerator;
	hr = CoCreateInstance(
		__uuidof(MMDeviceEnumerator),
		NULL,
		CLSCTX_ALL,
		__uuidof(IMMDeviceEnumerator),
		reinterpret_cast<void**>(cpDeviceEnumerator.GetAddressOf())
	);
	if (FAILED(hr)) {
		// Failed to get IMMDevice Enumerator
		return -1;
	}

	/* Step 2: get default endpoint render device (system audio) */
	hr = cpDeviceEnumerator.Get()->GetDefaultAudioEndpoint(eRender, eConsole, cpRenderDevice.GetAddressOf());
	if (FAILED(hr)) {
		// Failed to get default render device (system audio)
		return -2;
	}

	/* Step 3: activate audio client */
	hr = cpRenderDevice->Activate(
		__uuidof(IAudioClient),
		CLSCTX_ALL,
		NULL,
		reinterpret_cast<void**>(cpAudioClient.GetAddressOf())
	);
	if (FAILED(hr)) {
		// Failed to activate audio client
		return -3;
	}

	/* Step 4: get mix format of system audio */
	hr = cpAudioClient->GetMixFormat(&pWaveFormat);
	if (FAILED(hr)) {
		// Failed to get mix format of system audio
		return -4;
	}

	/* Step 5: initialize audio client */
	hr = cpAudioClient->Initialize(
		AUDCLNT_SHAREMODE_SHARED,
		AUDCLNT_STREAMFLAGS_LOOPBACK,
		(10 * 1000 * 1000),				// Buffer duration in 100ns-units.
		0,
		pWaveFormat,
		NULL
	);
	if (FAILED(hr)) {
		// Failed to initialize audio client
		return -5;
	}

	/* Step 6: get a capture audio client for recording system audio */
	hr = cpAudioClient->GetService(__uuidof(IAudioCaptureClient), reinterpret_cast<void**>(cpAudioCaptureClient.GetAddressOf()));
	if (FAILED(hr)) {
		// Failed to get a capture audio client for recording system audio
		return -6;
	}

	return 0;
}