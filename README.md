Sample Code for Integrating OpenCV with a BlackMagicDevice (Windows). The BlackMagicDevice used in this project is a [UltraStudio Recorder 3G](https://www.blackmagicdesign.com/products/ultrastudio/techspecs/W-DLUS-12).

---

<!-- PROJECT LOGO -->
<br />
<p style="text-align: center" align="center">
  <a href="https://github.com/kana800/BlackMagicOpenCV">
  <img src=".images/blackmagicopencv.png" alt="image logo"> 
  </a>

  <h3 align="center">BlackMagicOpenCV</h3>
  <p align="center">
    <br />
    <a href="https://github.com/kana800/BlackMagicOpenCV/issues">Report Bug</a>
    ·
    <a href="https://github.com/kana800/BlackMagicOpenCV/issues">Request Feature</a>
  </p>
</p>

---

### Setting Up BlackMagic SDK

- Download the [BlackMagic SDK Developer Kit 12.4](https://www.blackmagicdesign.com/developer/) 
- After Extracting the SDK, Move `include` directory in the `Win` folder into your project folder.
- Edit the `DECKLINK_SDK_INCLUDE_DIR` in the `CMakeLists.txt` to the correct location.

### [Setting Up OpenCV](https://www.youtube.com/watch?v=m9HBM1m_EMU)

- Edit the `OpenCV_DIR` in the `CMakeLists.txt`

### Build

You can build the project through **Visual Studio Code** or execute `build.bat` file. Makesure to **edit** the directories of the `OpenCV_DIR` and `DeckLinkSDK_Include_DIR` in the `build.bat` file. After building you can find the executable in the `build` directory in the `release` folder. 

### Run/Test

The test were done using [UltraStudio Recorder 3G](https://www.blackmagicdesign.com/products/ultrastudio/techspecs/W-DLUS-12) with a `SDI` input. Makesure to select the correct `displayMode` and `pixelFormat` for your *input feed*. To check if the *input feed* is compatible you can use `IDeckLinkDevice::doesItSupportDisplayMode` or `IDeckLinkInput::DoesSupportVideoMode` method.

```c++
/*BlackMagicOpenCV.cpp*/
capture->startDeckLinkCapture(
    true,
    bmdModeHD1080i5994, /*displayMode*/
    bmdFormat8BitYUV /*pixelFormat*/);
```

---

### DeckLink API

DeckLink API provides two ways to find a device. You can use:
- IDeckLinkIterator
- IDeckLinkDiscovery

#### Video Capture, How it works? 

> Video Capture operates in a push model with each video frame delivered to an `IDeckLinkInputCallback` object interface.

For Video Capture, You need to create a *input callback* class with subclass of `IDeckLinkInputCallback` (`DeckLinkInputDevice`). To *start capture*. We need to: 
- Query `IDeckLinkInput` from `IDeckLink`
- Register Input Callback Class
- Enable Video and Audio Input
- Start Input Streams
    

In `IDeckLinkInputCallback` class, `VideoInputFrameArrived` method is called when a video input frame or an audio packet has arrived. This method is implemented in `DeckLinkInputDevice::VideoInputFrameArrived`.

In our program, the **conversion of IDeckLinkVideoFrame to "OpenCV" frames**  is done inside `DeckLinkInputDevice::VideoInputFrameArrived` method. Due to this there is slight delay in the video feed. After the processing is completed we will clone the **"OpenCV" frame** to the `cv_frame` attribute. The user can obtain content of the `cv_frame` using the `DeckLinkInputDevice::getFrame` method. To avoid any race conditions we will be using **mutex**.  


---

### References

- [js-John BlackMagicDeckLinkOpenCV repo](https://github.com/js-john/Blackmagic-DeckLink-Opencv)
- [ull-isaatc blackmagic-test repo](https://github.com/ull-isaatc/blackmagic-test)
- [BlackMagic Developer Capture and Playback Sample Code](https://www.blackmagicdesign.com/developer/product/capture-and-playback)
