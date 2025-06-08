#ifndef IHANDLER_H
#define IHANDLER_H

#include "IWorker.h"
#include "TYPES/QueueWrapper.h"

// Interface IHandler for handlers
class IHandler : public IWorker {
public:
	// Destructor
	virtual ~IHandler() = default;
protected:
	// Constructor
	IHandler(QueueWrapper* pProceededData, QueueWrapper* pRecordedData)
		: pProceededData(pProceededData), pRecordedData(pRecordedData)
			{ }

	// RecordedData, that we receive from Recorder
	QueueWrapper* pRecordedData;

	// ProceededData after processing RecorderData
	QueueWrapper* pProceededData;
};

#endif // IHANDLER_H