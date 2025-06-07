#ifndef IHANDLER_H
#define IHANDLER_H

#include "IRecorder.h"

// Interface IHandler for handlers
class IHandler : public IRecorder {
public:
	// Constructor
	IHandler(QueueWrapper* pProceededData, QueueWrapper* pRecordedData) 
		: pProceededData(pProceededData),
			IRecorder(pRecordedData)
				{ }

	// Destructor
	virtual ~IHandler() = default;
private:
	// ProceededData after processing RecorderData
	QueueWrapper* pProceededData;
};

#endif // IHANDLER_H