#include "DeckLinkInputDevice.h"

DeckLinkInputDevice* DeckLinkInputDevice::CreateInstance(
    com_ptr<IDeckLink> &device){
    /**/
    return new DeckLinkInputDevice(device);       
}

DeckLinkInputDevice::DeckLinkInputDevice(
    com_ptr<IDeckLink> &device): 
    m_refCount(1),
    m_deckLink(device),
    m_deckLinkInput(IID_IDeckLinkInput, device),
    m_deckLinkConfig(IID_IDeckLinkConfiguration, device),
    m_currentlyCapturing(false){
}

DeckLinkInputDevice::~DeckLinkInputDevice(){
	/*summary: stops the decklinkcapture
     *if its running*/
    if (m_currentlyCapturing) stopDeckLinkCapture();
}


bool DeckLinkInputDevice::initializeDeckLinkDevice(){
    /*summary: initialize the settings 
     *needed for the decklink device and doing some checks
     *return -> TRUE (success) or FALSE (failed)*/

    /*provide details about capabilities
     *of a profile for a decklink card*/
    com_ptr<IDeckLinkProfileAttributes> deckLinkAttributes(
        IID_IDeckLinkProfileAttributes, m_deckLink);
    dlbool_t attributeFlag;

    /*checking if automatic input mode
     *is supported by the device*/
    if (deckLinkAttributes->GetFlag(
        BMDDeckLinkSupportsInputFormatDetection, &attributeFlag) == S_OK){
        m_supportInputFormatDetection = attributeFlag;
    } else {
        m_supportInputFormatDetection = false;
    }

    return true;
}

void DeckLinkInputDevice::startDeckLinkCapture(
    bool autodetectinputmode,
    BMDDisplayMode displayMode,
    BMDPixelFormat pixelFormat){
    /*summary: (Entry Point) Register 
     *Input Callback Class and Enable Video
     *and Audio Input and Starts Input Streams
     *args: bool autodetectinputmode -> detect 
     * bmd display mode automatically
     * BMDDisplayMode displayMode -> preferred 
     * display mode for the device
     * BMDPixelFormat pixelFormat -> preferred
     * pixel format for the device. Check 
     * Compatibility with doesItSupportDisplayMode method*/

    if (verbose){
        std::cout << "Start DeckLink Capture" << std::endl;
    }

    /*assigning private variables with the arguments*/
    m_autoDetectInputMode = autodetectinputmode;
    m_displayMode = displayMode;
    m_pixelFormat = pixelFormat;

    BMDVideoInputFlags videoInputFlgas = bmdVideoInputFlagDefault;

    if (m_supportInputFormatDetection && m_autoDetectInputMode){
        videoInputFlgas |= bmdVideoInputEnableFormatDetection;
    }

    m_deckLinkInput->SetCallback(this);

    if (m_deckLinkInput->EnableVideoInput(m_displayMode,
        pixelFormat,
        bmdVideoInputEnableFormatDetection) != S_OK){
            std::cerr << "Failed to Enable Video Input" << std::endl;
            return;
    }

    if (m_deckLinkInput->StartStreams() != S_OK){
        std::cerr << "Failed to Start Streams" << std::endl;
        return;
    }

    m_currentlyCapturing = true;
}

void DeckLinkInputDevice::printDeviceName(){
    /*summary: prints the name of the 
     *current connected device*/
    dlstring_t modelname;
    m_deckLink->GetDisplayName(&modelname);
    printf("Device: %s\n",
        DlToStdString(modelname).c_str());
    DeleteString(modelname);
    return;
}

bool DeckLinkInputDevice::doesItSupportDisplayMode(
    BMDVideoConnection connectionType,
    BMDDisplayMode requestedDisplayMode,
    BMDPixelFormat requestedPixelFormat){
    /*summary: returns whether the device is 
     *supported/compatible with requestedDisplayMode
     *and requestedPixelFormat.
     *args: BMDVideoConnection connectionType -> 
     * input connection to check (see BMDVideoConnection for details)
     * BMDDisplayMode requestedDisplayMode -> displaymode to check
     * BMDPixelFormat requestedPixelFormat -> pixelformat to check
     *return: TRUE (SUPPORTED) or FALSE (NOTSUPPORTED)*/

    dlbool_t supported;
    if (m_deckLinkInput->DoesSupportVideoMode(
        connectionType, requestedDisplayMode, requestedPixelFormat,
        bmdNoVideoInputConversion, bmdSupportedVideoModeDefault,
        nullptr, &supported) != S_OK){
            printf("failed to CheckSupportedMode");
            return false;
    }
    return supported; 
}

HRESULT DeckLinkInputDevice::VideoInputFrameArrived(
    IDeckLinkVideoInputFrame* videoFrame,
    IDeckLinkAudioInputPacket* /*unused*/){
    /*summary: called when the video frame is arrived;
     *videoFrame is only valid for the callback duration
     *args: videoFrame -> video frame that has arrived
     * audioPacket -> audio pack that has arrived
     *return: E_FAIL -> failure
     *  S_OK -> success  
     */    
    if (videoFrame == nullptr) return S_OK;

    std::lock_guard<std::mutex> lock(m_deckLinkInputMutex);
    /*Gather Video Information*/
    void* data;
    videoFrame->GetBytes(&data);
    const auto width = (int32_t)videoFrame->GetWidth();
    const auto height = (int32_t)videoFrame->GetHeight();
    const auto rowBytes = (int32_t)videoFrame->GetRowBytes();

    /*converts IDeckLinkVideoFrame into opencv frame*/
    cv::Mat image(height, width, CV_8UC3);

    if (m_pixelFormat == bmdFormat8BitYUV){
        cv::Mat uyvy(height, width, CV_8UC2, data, rowBytes);
        cv::cvtColor(uyvy, image, cv::COLOR_YUV2BGR_UYVY);
    } else if (m_pixelFormat == bmdFormat8BitBGRA){
        cv::Mat uyvy(height, width, CV_8UC4, data);
        cv::cvtColor(uyvy, image, cv::COLOR_BGRA2BGR);
    }
    cv_frame = image.clone();

    return S_OK;
}

cv::Mat DeckLinkInputDevice::getFrame(){
    /*summary:GETTER for the cv_frame*/
    std::unique_lock<std::mutex> lock(m_deckLinkInputMutex);
    return cv_frame;
}

void DeckLinkInputDevice::stopDeckLinkCapture(){
    /*summary: stops the decklink stream*/
    if (verbose){
        std::cout << "Stop DeckLink Capture" << std::endl;
    }

    if (m_deckLink){ 
        /*stopping the stream*/
        m_deckLinkInput->StopStreams();
        m_deckLinkInput->DisableVideoInput();
        /*deleting the callback*/
        m_deckLinkInput->SetCallback(nullptr);
    }
    m_currentlyCapturing = false;
}

HRESULT DeckLinkInputDevice::VideoInputFormatChanged(
    BMDVideoInputFormatChangedEvents notificationEvents,
    IDeckLinkDisplayMode* newDisplayMode,
    BMDDetectedVideoInputFormatFlags detectedSignalFlags){
    /**/
    const BMDDisplayMode newBMDDisplayMode = newDisplayMode->GetDisplayMode();
    m_displayMode = newBMDDisplayMode;

    if (verbose){
        std::cout << "video input format changed: " << std::endl;
    }

    std::lock_guard<std::mutex> lock(m_deckLinkInputMutex);

    m_deckLinkInput->StopStreams();
    m_deckLinkInput->EnableVideoInput(
        newBMDDisplayMode,
        m_pixelFormat,
        bmdVideoInputEnableFormatDetection);
    m_deckLinkInput->StartStreams();

	return S_OK;
}

HRESULT DeckLinkInputDevice::QueryInterface(
    REFIID iid, LPVOID* ppv){
    /**/
	HRESULT result = S_OK;

	if (iid == IID_IUnknown){
		*ppv = this;
		AddRef();
	}
	else if (iid == IID_IDeckLinkInputCallback){

		*ppv = static_cast<IDeckLinkInputCallback*>(this);
		AddRef();
	} else {
        *ppv = nullptr;
		result = E_NOINTERFACE;
	}

	return result;
}

ULONG DeckLinkInputDevice::AddRef(){
    return ++m_refCount;
}

ULONG DeckLinkInputDevice::Release(){
	ULONG refCount = --m_refCount;
	if (refCount == 0)
		delete this;
	return refCount;
}