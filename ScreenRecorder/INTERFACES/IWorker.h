#ifndef IWORKER_H
#define IWORKER_H

// Abstract interface for all worker modules
class IWorker {
public:
	virtual ~IWorker() = default;

	// Initializes the worker. Returns 0 on success.
	virtual int Initialize() = 0;

	// Starts the worker's internal processing thread
	virtual void StartThread() = 0;

	// Signals the worker to stop gracefully
	virtual void EndRequest() = 0;
};

#endif // IWORKER_H