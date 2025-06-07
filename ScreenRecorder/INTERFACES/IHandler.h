#ifndef _IHANDLER_H
#define _IHANDLER_H

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
	QueueWrapper* pProceededData;
};

#endif // _IHANDLER_H