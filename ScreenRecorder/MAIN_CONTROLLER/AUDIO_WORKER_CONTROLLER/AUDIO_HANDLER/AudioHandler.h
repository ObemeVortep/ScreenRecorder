#ifndef SYSAUDIO_HANDLER_H
#define SYSAUDIO_HANDLER_H

#include "INTERFACES/IWorker.h"

#include "TYPES/AudioRecordedData.h"

class AudioHandler : public IWorker {
public:
	// Constructor
	AudioHandler();
	// Destructor
	~AudioHandler();

	// Initialize AudioHandler instances
	int Initialize() override;

	// Start sysaudio capturing
	void StartThread() override;

	// End sysaudio capturing
	void EndRequest() override;

};

#endif // SYSAUDIO_HANDLER_H