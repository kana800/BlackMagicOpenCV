:: small script to build the project
if "%DeckLinkSDK_Include_DIR%"=="" (set DeckLinkSDK_Include_DIR=%cd%\include)

if "%OpenCV_DIR%"=="" (set OpenCV_DIR=C:\opencv\build\install)

mkdir build
cd build
cmake -G "Visual Studio 17 2022" -DDECKLINK_SDK_INCLUDE_DIR="%DeckLinkSDK_Include_DIR%" -DOpenCV_STATIC=ON -DOpenCV_RUNTIME=vc17 ..
cmake --build . --config Release