cmake_minimum_required(VERSION 3.22.1)

project(ProgramRunner VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
# OpenCV
pkg_check_modules(OPENCV REQUIRED opencv4)

# Tesseract
pkg_check_modules(TESSERACT REQUIRED tesseract lept)

include_directories(
        HIDGController
        Capture
        ${OpenCV_INCLUDE_DIRS} # OpenCV
        ${TESSERACT_INCLUDE_DIRS} # Tesseract
        ../include/crow/include # Crow Headers
)

set(SOURCES
        src/main.cpp
        src/HIDGController/HIDGController.cpp
        src/HIDGController/Gamepad.cpp
        src/Capture/Capture.cpp
        src/Capture/ImageRecognition.cpp
        src/Capture/TextRecognition.cpp
)
add_executable(ProgramRunner ${SOURCES})

target_link_libraries(ProgramRunner
        ${OPENCV_LIBRARIES}
        ${TESSERACT_LIBRARIES}
)