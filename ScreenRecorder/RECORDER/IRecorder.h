#ifndef _IRECORDER_H
#define _IRECORDER_H

// Abstract interface for all recorder modules (e.g., screen, audio, mic)
class IRecorder {
public:
	virtual ~IRecorder() = default;

	// Initializes the recorder. Returns 0 on success.
	virtual int Initialize() = 0;

	// Starts the recorder's internal processing thread
	virtual void StartThread() = 0;

	// Signals the recorder to stop gracefully
	virtual void EndRequest() = 0;
};

#endif // _IRECORDER_H
