#include "main.h"
#include <iostream>

int main() {
	WorkerController WorkerController;
	unsigned int uiThreads = WorkerController.StartThreads();
	if (uiThreads == 0x03) {
		std::cout << "All threads are inited." << std::endl;
	}

	while (true) {
		std::this_thread::sleep_for(std::chrono::seconds(1000));
	}
}