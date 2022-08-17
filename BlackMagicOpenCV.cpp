// BlackMagicOpenCV.cpp : Defines the entry point for the application.

#include "BlackMagicOpenCV.h"
#include "DeckLinkAPI.h"
#include "DeckLinkInputDevice.h"
#include "DeviceManager.h"
#include "platform.h"

#include <opencv2/opencv.hpp>



int main(int argc, char *argv[]){

	/*setting up devicemanager and 
	 *grabbing a decklink instance*/
	DeviceManager deviceManager;
	auto deckLink = deviceManager.getDeckLink();

	auto capture = DeckLinkInputDevice::CreateInstance(deckLink); 
	capture->printDeviceName();
	capture->initializeDeckLinkDevice();
	capture->startDeckLinkCapture(true, bmdModeHD1080i5994, bmdFormat8BitYUV);

	while(true){
		if (cv::waitKey(30) == 27){
			break;
		}
		cv::Mat frame = capture->getFrame();
		if (!frame.empty()){
			cv::imshow("decklink", frame);
		}
	}

	return 0;

}
