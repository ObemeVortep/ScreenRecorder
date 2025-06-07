#ifndef QUEUE_WRAPPER_H
#define QUEUE_WRAPPER_H

#include <queue>
#include <condition_variable>
#include <mutex>

// Shared data buffer for all IRecorder implementations (accessed via reference).
class QueueWrapper {
public:
	// Adds a new data block to the queue and notifies one waiting thread.
	inline void Push(std::vector<unsigned char> vDataSrc) {
		{
			std::lock_guard<std::mutex> DataQueueGuard(QueueMutex);
			DataQueue.push(std::move(vDataSrc));
			QueueCondition.notify_one(); // Wake up one waiting thread
		}
	}

	// Waits until the queue is non-empty (used by consumer threads).
	// Retrieves and removes the front element from the queue.
	// The caller must ensure the queue is not empty before calling.
	inline void WaitFrontAndPop(std::vector<unsigned char>& vDataDst) {
		{
			std::unique_lock<std::mutex> ConditionGuard(QueueMutex);
			QueueCondition.wait(ConditionGuard, [this]() { return !DataQueue.empty(); });
			vDataDst = std::move(DataQueue.front());
			DataQueue.pop();
		}
	}

	// Waits until the queue is non-empty (used by consumer threads).
	inline void Wait() {
		{
			std::unique_lock<std::mutex> ConditionGuard(QueueMutex);
			QueueCondition.wait(ConditionGuard, [this]() { return !DataQueue.empty(); });
		}
	}

private:
	// Thread-safe queue of recorded data blocks (e.g., frames)
	std::queue<std::vector<unsigned char>> DataQueue;

	// Mutex protecting access to the data queue
	std::mutex QueueMutex;

	// Condition variable to notify/wait for data availability
	std::condition_variable QueueCondition;
};

#endif // QUEUE_WRAPPER_H
