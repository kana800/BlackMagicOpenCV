Sample Code for Integrating OpenCV with a BlackMagicDevice (Windows). The BlackMagicDevice used in this project is a [UltraStudio Recorder 3G](https://www.blackmagicdesign.com/products/ultrastudio/techspecs/W-DLUS-12).

---

### Setting Up BlackMagic SDK

- Download the [BlackMagic SDK Developer Kit 12.4](https://www.blackmagicdesign.com/developer/) 
- After Extracting the SDK, Move `include` directory in the `Win` folder into your project folder.
- Edit the `DECKLINK_SDK_INCLUDE_DIR` in the `CMakeLists.txt` to the correct location.

### [Setting Up OpenCV](https://www.youtube.com/watch?v=m9HBM1m_EMU)

- Edit the `OpenCV_DIR` in the `CMakeLists.txt`

### Build

You can build the project through **Visual Studio Code** or execute `build.bat` file. Makesure to **edit** the directories of the `OpenCV_DIR` and `DeckLinkSDK_Include_DIR` in the `build.bat` file. After building you can find the executable in the `build` directory in the `release` folder. 

---

### DeckLink API

DeckLink API provides two ways to find a device. You can use:
- IDeckLinkIterator
- IDeckLinkDiscovery

#### Video Capture, How it works? 




---

### References

- [js-John BlackMagicDeckLinkOpenCV repo](https://github.com/js-john/Blackmagic-DeckLink-Opencv)
- [ull-isaatc blackmagic-test repo](https://github.com/ull-isaatc/blackmagic-test)
- [BlackMagic Developer Capture and Playback Sample Code](https://www.blackmagicdesign.com/developer/product/capture-and-playback)