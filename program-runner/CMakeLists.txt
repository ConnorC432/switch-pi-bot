cmake_minimum_required(VERSION 3.22.1)

project(ProgramRunner VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_definitions(-DCROW_ENABLE_ASIO_STANDALONE)

# Include directories
include_directories(
    ## Classes
    HIDGController
    Capture
    Programs

    ## System includes
    ../include/crow/include
    /usr/include
    /usr/include/opencv4
    /usr/include/tesseract
)

# Source files
set(SOURCES
    src/main.cpp
    src/HIDGController/HIDGController.cpp
    src/HIDGController/Gamepad.cpp
    src/Capture/Capture.cpp
    src/Capture/ImageRecognition.cpp
    src/Capture/TextRecognition.cpp
    src/ProgramWorker.cpp
    src/ProgramRegistry.cpp
)

file(GLOB_RECURSE PROGRAM_SOURCES "src/Programs/*.cpp")
list(APPEND SOURCES ${PROGRAM_SOURCES})

message(STATUS "Program sources: ${PROGRAM_SOURCES}")

# Create executable
add_executable(ProgramRunner ${SOURCES})

# Libraries (link against system libraries directly)
target_link_libraries(ProgramRunner PRIVATE
    opencv_core
    opencv_imgproc
    opencv_imgcodecs
    opencv_highgui
    tesseract
    lept
    pthread
)
