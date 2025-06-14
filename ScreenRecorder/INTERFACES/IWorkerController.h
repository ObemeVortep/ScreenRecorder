#ifndef IWORKER_CONTROLLER_H
#define IWORKER_CONTROLLER_H

#include "TYPES/SharedQueue.h"
#include <memory>

#include "IWorker.h"
#include <thread>

class IWorkerController {
public:
	// Destructor
	virtual ~IWorkerController() = default;

	// Initialize all worker threads
	virtual std::shared_ptr<SharedQueue<std::vector<unsigned char>>> Initialize() = 0;

	// Start all worker threads
	virtual void StartThreads() = 0;
protected:
	// Internal initialize of workers
	virtual int InitializeWorkers(IWorker* pRecorder, IWorker* pHandler) {
		// RECORDERS MUST BE INITIALIZED BEFORE HANDLERS
		if (pRecorder->Initialize() != 0) {
			return -1;
		}
		if (pHandler->Initialize() != 0) {
			return -2;
		}

		return 0;
	}

	// Internal start workers threads
	virtual void StartWorkers(IWorker* pRecorder, IWorker* pHandler, std::jthread& recorderThread, std::jthread& handlerThread) {
		// HANDLERS MUST BE STARTED BEFORE RECORDERS
		handlerThread = std::jthread(&IWorker::StartThread, pHandler);
		recorderThread = std::jthread(&IWorker::StartThread, pRecorder);
	}

};

#endif // IWORKER_CONTROLLER_H