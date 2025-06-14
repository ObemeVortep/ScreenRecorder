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
};

#endif // IWORKER_CONTROLLER_H