/*subclass of IDeckLinkInputCallback
 *IDeckLinkInputCallback object interface is a callback
 *class which is called for each captured frame*/
#include "DeckLinkAPI.h"
#include "com_ptr.h"
#include "platform.h"

#include <opencv2/opencv.hpp>

#include <iostream>
#include <mutex>
#include <condition_variable>

class DeckLinkInputDevice:
    public IDeckLinkInputCallback {

    public:
        static DeckLinkInputDevice* CreateInstance(
            com_ptr<IDeckLink>& device);
        
        /*controlling the decklink capture*/
        bool initializeDeckLinkDevice();
        void startDeckLinkCapture(
            bool autodetectinputmode,
            BMDDisplayMode bmdDisplayMode,
            BMDPixelFormat pixelFormat);
        void stopDeckLinkCapture();

        /*getters for device information*/
        void printDeviceName();
        bool doesItSupportDisplayMode(
            BMDVideoConnection connectionType,
            BMDDisplayMode requestedDisplayMode,
            BMDPixelFormat requestedPixelFormat);

        /*getters for the decklink attributes (instances)*/
        com_ptr<IDeckLink> getDeckLinkInstance(void) const { return m_deckLink; };
        com_ptr<IDeckLinkInput> getDeckLinkInputInstance(void) const { return m_deckLinkInput; };
        com_ptr<IDeckLinkConfiguration> getDeckLinkConfigInstance(void) const { return m_deckLinkConfig; };

        cv::Mat getFrame();

    private:

        DeckLinkInputDevice(com_ptr<IDeckLink> &device);
	    virtual ~DeckLinkInputDevice();

        /*IUnknown*/
        virtual HRESULT  QueryInterface(REFIID iid, LPVOID* ppv) override;
        virtual ULONG  AddRef() override;
        virtual ULONG   Release() override;

        /*IDeckLinkInput Callback*/
        virtual HRESULT VideoInputFrameArrived(
            IDeckLinkVideoInputFrame* videoFrame,
            IDeckLinkAudioInputPacket* audioPacket) override;
        
        virtual HRESULT  VideoInputFormatChanged(
            BMDVideoInputFormatChangedEvents notificationEvents,
            IDeckLinkDisplayMode* newDisplayMode,
            BMDDetectedVideoInputFormatFlags detectedSignalFlags
        ) override;

 
    private:
        std::mutex m_deckLinkInputMutex;
        std::condition_variable m_videoFrameArrival;
        std::atomic<ULONG> m_refCount;

        com_ptr<IDeckLink> m_deckLink;
        com_ptr<IDeckLinkInput> m_deckLinkInput;
        com_ptr<IDeckLinkConfiguration> m_deckLinkConfig;

        BMDDisplayMode m_displayMode;
        BMDPixelFormat m_pixelFormat;
        
        bool m_autoDetectInputMode;
        bool m_supportInputFormatDetection;
        bool m_currentlyCapturing;

        bool verbose = false;

        cv::Mat cv_frame;
};