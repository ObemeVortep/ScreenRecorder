#include "AudioRecorder.h"

// Constructor
AudioRecorder::AudioRecorder(std::shared_ptr<AudioRecordedData<std::vector<unsigned char>>> spRecordedAudio, EDataFlow eType)
	: spRecordedAudio(spRecordedAudio), deviceType(eType),
		pWaveFormat(NULL)
			{ }

// Destructor
AudioRecorder::~AudioRecorder() {
	if (pWaveFormat) {
		// pWaveFormat must be cleaned using CoTaskMemFree()
		CoTaskMemFree(pWaveFormat);
	}
}


// Initialize AudioRecorder instances
int AudioRecorder::Initialize() {
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

	/* Step 2: get default endpoint audio device */
	hr = cpDeviceEnumerator.Get()->GetDefaultAudioEndpoint(deviceType, eConsole, cpDevice.GetAddressOf());
	if (FAILED(hr)) {
		// Failed to get default audio device 
		return -2;
	}

	/* Step 3: activate audio client */
	hr = cpDevice->Activate(
		__uuidof(IAudioClient),
		CLSCTX_ALL,
		NULL,
		reinterpret_cast<void**>(cpAudioClient.GetAddressOf())
	);
	if (FAILED(hr)) {
		// Failed to activate audio client
		return -3;
	}

	/* Step 4: get mix format of audio */
	hr = cpAudioClient->GetMixFormat(&pWaveFormat);
	if (FAILED(hr)) {
		// Failed to get mix format of audio
		return -4;
	}

	/* Step 5: initialize audio client */
	// Define if loopback needed
	DWORD dwFlags = (deviceType == eRender) ? AUDCLNT_STREAMFLAGS_LOOPBACK : 0;

	hr = cpAudioClient->Initialize(
		AUDCLNT_SHAREMODE_SHARED,
		dwFlags,
		(10 * 1000 * 1000),				// Buffer duration in 100ns-units.
		0,
		pWaveFormat,
		NULL
	);
	if (FAILED(hr)) {
		// Failed to initialize audio client
		return -5;
	}

	/* Step 6: get a capture audio client for recording audio */
	hr = cpAudioClient->GetService(__uuidof(IAudioCaptureClient), reinterpret_cast<void**>(cpAudioCaptureClient.GetAddressOf()));
	if (FAILED(hr)) {
		// Failed to get a capture audio client for recording audio
		return -6;
	}

	/* Step 7: initialize WAVEFORMAT in spRecordedAudio */
	spRecordedAudio->SetAudioFormat(pWaveFormat);

	return 0;
}
