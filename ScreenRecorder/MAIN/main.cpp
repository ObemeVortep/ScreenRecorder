#include <iostream>
#include "main.h"

int main()
{
	ScreenRecorder ScreenRecorder;
	if (ScreenRecorder.InitCapturingInterfaces() == 0) {
		std::cout << "good" << std::endl;
	}
}
