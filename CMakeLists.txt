# CMakeList.txt : CMake project for BlackMagicOpenCV
cmake_minimum_required (VERSION 3.8)

project ("BlackMagicOpenCV")

set( OpenCV_DIR "C:/opencv/build/install" )
set( DECKLINK_SDK_INCLUDE_DIR "C:/DEV/BlackMagicOpenCV/include" )
set(OpenCV_STATIC ON)
FIND_PACKAGE( OpenCV REQUIRED )


SET(APP_HEADERS
	com_ptr.h
	BlackMagicOpenCV.h
	DeckLinkInputDevice.h
        DeviceManager.h
	)

SET(APP_SOURCES
	BlackMagicOpenCV.cpp
	DeckLinkInputDevice.cpp
	${APP_HEADERS}
)

SET(PLATFORM_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/DeckLinkAPI_i.c
    ${CMAKE_CURRENT_BINARY_DIR}/DeckLinkAPI.h
)

LIST(APPEND PLATFORM_LIBRARIES comsuppw.lib)

# Build a multi-threaded binary
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

# Compile DeckLink API IDL to build directory
SET(MIDL_OUTPUT_SOURCE "${CMAKE_CURRENT_BINARY_DIR}\\DeckLinkAPI_i.c")
SET(MIDL_OUTPUT_HEADER "${CMAKE_CURRENT_BINARY_DIR}\\DeckLinkAPI.h")
SET(DECKLINK_API_IDL "${DECKLINK_SDK_INCLUDE_DIR}\\DeckLinkAPI.idl")
SET(DECKLINK_SDK_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR})


IF (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)")
        SET(MIDL_MACHINE win32)
ELSE ()
        SET(MIDL_MACHINE amd64)
ENDIF ()

SET(MIDL_OUTPUT
    ${MIDL_OUTPUT_SOURCE}
    ${MIDL_OUTPUT_HEADER}
    )

# Compile the DeckLinkAPI.idl file (to generate DeckLinkAPI.h and DeckLinkAPI_i.c)
ADD_CUSTOM_COMMAND(
    OUTPUT ${MIDL_OUTPUT}
    DEPENDS ${DECKLINK_API_IDL}
    COMMAND midl /env ${MIDL_MACHINE} /header ${MIDL_OUTPUT_HEADER} ${DECKLINK_API_IDL}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

SET_SOURCE_FILES_PROPERTIES(${MIDL_OUTPUT} PROPERTIES GENERATED TRUE)

ADD_CUSTOM_TARGET(MIDL_Target
    DEPENDS ${MIDL_OUTPUT}
)

# Add C++ projects
ADD_EXECUTABLE(${PROJECT_NAME}
        ${APP_SOURCES}
        ${PLATFORM_SOURCES}
        )


TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE
        ${DECKLINK_SDK_INCLUDE_DIR}
        ${APP_SOURCES}
        ${OpenCV_INCLUDE_DIRS}
        )


TARGET_LINK_LIBRARIES(${PROJECT_NAME}
        ${OpenCV_LIBS}
        ${PLATFORM_LIBRARIES}
        )