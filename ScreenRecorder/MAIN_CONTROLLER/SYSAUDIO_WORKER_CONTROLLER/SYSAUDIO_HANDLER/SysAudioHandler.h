#ifndef SYSAUDIO_HANDLER_H
#define SYSAUDIO_HANDLER_H

#include "INTERFACES/IWorker.h"

class SysAudioHandler : public IWorker {
public:
	// Constructor
	SysAudioHandler();
	// Destructor
	~SysAudioHandler();

	// Initialize SysAudioHandler instances
	int Initialize() override;

	// Start sysaudio capturing
	void StartThread() override;

	// End sysaudio capturing
	void EndRequest() override;

private:

};

#endif // SYSAUDIO_HANDLER_H