#ifndef AUDIO_RECORDED_DATA_H
#define AUDIO_RECORDED_DATA_H

#include "TYPES/SharedQueue.h"
#include <audioclient.h>

template <class T>
class AudioRecordedData : public SharedQueue<T> {
public:
	// Constructor
	AudioRecordedData() : pAudioFormat(nullptr) { }

	// Set audio format
	inline void SetAudioFormat(WAVEFORMATEX* pAudioFormatOrig) {
		pAudioFormat = pAudioFormatOrig;
	}

	// Get audio format
	inline WAVEFORMATEX* GetAudioFormat() {
		return pAudioFormat;
	}
private:
	WAVEFORMATEX* pAudioFormat;
};

#endif // AUDIO_RECORDED_DATA_H