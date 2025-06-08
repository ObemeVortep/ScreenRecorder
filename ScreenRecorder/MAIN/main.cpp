#include "main.h"

int main() {
	// Main connection between WorkerController and VideoCreator
	VideoPipelineBuffer VideoPipelineBuffer;

	// WorkerController manages all data collection threads for video creation
	WorkerController WorkerController(&VideoPipelineBuffer);
	unsigned int uiActiveThreads = WorkerController.StartThreads();

	// VideoCreator creates video from VideoPipelineBuffer data in real time
	VideoCreator VideoCreator(&VideoPipelineBuffer, uiActiveThreads);
	VideoCreator.StartVideoCreation();

	while (true) {
		std::this_thread::sleep_for(std::chrono::seconds(1000));
	}
}