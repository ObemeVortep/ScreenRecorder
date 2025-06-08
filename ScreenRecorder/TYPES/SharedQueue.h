#ifndef SHARED_QUEUE_H
#define SHARED_QUEUE_H

#include <queue>
#include <condition_variable>
#include <mutex>

// Shared thread-safe queue for all IWorker implementations (accessed via reference).
template <class T>
class SharedQueue {
public:
	// Adds a new data block to the queue and notifies one waiting thread.
	inline void Push(T vDataSrc) {
		{
			std::lock_guard<std::mutex> DataQueueGuard(QueueMutex);
			DataQueue.push(std::move(vDataSrc));
			QueueCondition.notify_one(); // Wake up one waiting thread
		}
	}

	// Waits until the queue is non-empty (used by consumer threads).
	// Retrieves and removes the front element from the queue.
	// The caller must ensure the queue is not empty before calling.
	inline void WaitFrontAndPop(T& vDataDst) {
		{
			std::unique_lock<std::mutex> ConditionGuard(QueueMutex);
			QueueCondition.wait(ConditionGuard, [this]() { return !DataQueue.empty(); });
			vDataDst = std::move(DataQueue.front());
			DataQueue.pop();
		}
	}

private:
	// Thread-safe queue of recorded data blocks (e.g., frames)
	std::queue<T> DataQueue;

	// Mutex protecting access to the data queue
	std::mutex QueueMutex;

	// Condition variable to notify/wait for data availability
	std::condition_variable QueueCondition;
};

#endif // SHARED_QUEUE_H