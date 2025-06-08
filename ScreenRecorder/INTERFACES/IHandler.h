#ifndef IHANDLER_H
#define IHANDLER_H

#include "IWorker.h"
#include "TYPES/SharedQueue.h"

// Interface IHandler for handlers
class IHandler : public IWorker {
public:
	// Destructor
	virtual ~IHandler() = default;
protected:
	// Constructor
	IHandler(SharedQueue<std::vector<unsigned char>>* pProcessedData, SharedQueue<std::vector<unsigned char>>* pRecordedData)
		: pProcessedData(pProcessedData), pRecordedData(pRecordedData)
			{ }

	// RecordedData, that we receive from Recorder
	SharedQueue<std::vector<unsigned char>>* pRecordedData;

	// ProceededData after processing RecorderData
	SharedQueue<std::vector<unsigned char>>* pProcessedData;
};

#endif // IHANDLER_H