#include "AudioHandler.h"

// Constructor implementation
AudioHandler::AudioHandler(std::shared_ptr<AudioRecordedData<std::vector<unsigned char>>> spRecordedSysAudio, std::shared_ptr<AudioRecordedData<std::vector<unsigned char>>> spRecordedMicAudio, std::shared_ptr<SharedQueue<std::vector<unsigned char>>> spProcessedAudio)
	: spRecordedSysAudio(spRecordedSysAudio), spRecordedMicAudio(spRecordedMicAudio), spProcessedAudio(spProcessedAudio)
		{ }

// Destructor implementation
AudioHandler::~AudioHandler() {

}

// Initialize AudioHandler instances
int AudioHandler::Initialize() {
	return 0; 
}