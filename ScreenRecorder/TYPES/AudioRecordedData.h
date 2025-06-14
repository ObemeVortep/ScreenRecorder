#ifndef AUDIO_RECORDED_DATA_H
#define AUDIO_RECORDED_DATA_H

#include "TYPES/SharedQueue.h"
#include <audioclient.h>

template <class T>
class AudioRecordedData : public SharedQueue<T> {
public:
	// Constructor
	AudioRecordedData() : audioFormat(NULL) { }

	// Set audio format
	inline void SetAudioFormat(WAVEFORMATEX* pAudioFormat) {
		std::memcpy(&audioFormat, pAudioFormat, sizeof(WAVEFORMATEX));
	}

	// Get audio format
	inline WAVEFORMATEX GetAudioFormat() {
		return audioFormat;
	}
private:
	WAVEFORMATEX audioFormat;
};

#endif // AUDIO_RECORDED_DATA_H