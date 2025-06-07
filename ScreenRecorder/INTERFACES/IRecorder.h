#ifndef IRECORDER_H
#define IRECORDER_H

#include "TYPES/QueueWrapper.h"

// Abstract interface for all recorder modules (e.g., screen, audio, mic)
class IRecorder {
public:
	IRecorder(QueueWrapper* pRecordedData) : pRecordedData(pRecordedData) {};

	virtual ~IRecorder() = default;

	// Initializes the recorder. Returns 0 on success.
	virtual int Initialize() = 0;

	// Starts the recorder's internal processing thread
	virtual void StartThread() = 0;

	// Signals the recorder to stop gracefully
	virtual void EndRequest() = 0;

protected:
	QueueWrapper* pRecordedData;
};

#endif // IRECORDER_H