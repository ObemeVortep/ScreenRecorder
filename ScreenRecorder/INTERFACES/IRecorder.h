#ifndef IRECORDER_H
#define IRECORDER_H

#include "IWorker.h"
#include "TYPES/QueueWrapper.h"

// Abstract interface for all recorder modules (e.g., screen, audio, mic)
class IRecorder : public IWorker {
public:
	virtual ~IRecorder() = default;

protected:
	IRecorder(QueueWrapper* pRecordedData) : pRecordedData(pRecordedData) {};

	// We save recorded data there
	QueueWrapper* pRecordedData;
};

#endif // IRECORDER_H