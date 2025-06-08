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
	IHandler(SharedQueue<std::vector<unsigned char>>* pProceededData, SharedQueue<std::vector<unsigned char>>* pRecordedData)
		: pProceededData(pProceededData), pRecordedData(pRecordedData)
			{ }

	// RecordedData, that we receive from Recorder
	SharedQueue<std::vector<unsigned char>>* pRecordedData;

	// ProceededData after processing RecorderData
	SharedQueue<std::vector<unsigned char>>* pProceededData;
};

#endif // IHANDLER_H