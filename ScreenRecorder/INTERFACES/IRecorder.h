#ifndef IRECORDER_H
#define IRECORDER_H

#include "IWorker.h"
#include "TYPES/SharedQueue.h"

// Abstract interface for all recorder modules (e.g., screen, audio, mic)
class IRecorder : public IWorker {
public:
	virtual ~IRecorder() = default;

protected:
	IRecorder(SharedQueue<std::vector<unsigned char>>* pRecordedData) : pRecordedData(pRecordedData) {};

	// We save recorded data there
	SharedQueue<std::vector<unsigned char>>* pRecordedData;
};

#endif // IRECORDER_H