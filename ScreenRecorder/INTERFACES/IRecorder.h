<<<<<<<< HEAD:ScreenRecorder/TYPES/QueueWrapper.h
#ifndef QUEUE_WRAPPER_H
#define QUEUE_WRAPPER_H
========
#ifndef IRECORDER_H
#define IRECORDER_H
>>>>>>>> 509424ead27b3ac795ffdcc5acb2061ce8979f31:ScreenRecorder/INTERFACES/IRecorder.h


private:
	// Thread-safe queue of recorded data blocks (e.g., frames)
	std::queue<std::vector<unsigned char>> DataQueue;

	// Mutex protecting access to the data queue
	std::mutex QueueMutex;

	// Condition variable to notify/wait for data availability
	std::condition_variable QueueCondition;
};

<<<<<<<< HEAD:ScreenRecorder/TYPES/QueueWrapper.h
#endif // QUEUE_WRAPPER_H
========
// Abstract interface for all recorder modules (e.g., screen, audio, mic)
class IRecorder {
public:
	IRecorder(QueueWrapper* pRecordedData) : pRecordedData(pRecordedData) {};

	virtual ~IRecorder() = default;

	// Initializes the recorder. Returns 0 on success.
	virtual int Initialize() = 0;

	// Starts the recorder's internal processing thread
	virtual void StartThread() = 0;

	// Signals the recorder to stop gracefully
	virtual void EndRequest() = 0;

protected:
	QueueWrapper* pRecordedData;
};

#endif // IRECORDER_H
>>>>>>>> 509424ead27b3ac795ffdcc5acb2061ce8979f31:ScreenRecorder/INTERFACES/IRecorder.h
