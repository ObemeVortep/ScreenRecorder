#include "main.h"
#include <iostream>

int main() {
	AppController appController;
	unsigned int uiThreads = appController.StartThreads();
	if (uiThreads == 0x01) {
		std::cout << "Only ScreenRecorder is inited." << std::endl;
	}

	while (true);
}