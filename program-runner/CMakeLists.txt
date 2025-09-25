cmake_minimum_required(VERSION 3.22.1)

project(ProgramRunner VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#add_definitions(-DCROW_ENABLE_ASIO_STANDALONE)

# Boost
find_package(Boost REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

# Crow
include_directories(
    ${CMAKE_SOURCE_DIR}/include/crow/include
)

# Mongo C/C++ driver
include_directories(
    /usr/include/mongocxx/v_noabi/
    /usr/include/bsoncxx/v_noabi/
)

# OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Tesseract
include_directories(
    /usr/include/tesseract
    /usr/include/leptonica
)

# Classes
include_directories(
    ${CMAKE_SOURCE_DIR}/HIDGController
    ${CMAKE_SOURCE_DIR}/Capture
    ${CMAKE_SOURCE_DIR}/Programs
    ${CMAKE_SOURCE_DIR}/Database
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
        src/Database/Programs.cpp
        src/Database/Programs.h
        src/Database/Database.cpp
        src/Database/Database.h
        src/Database/Programs.cpp
        src/Database/Programs.h
)

# Find and add all programs
file(GLOB_RECURSE PROGRAM_SOURCES "${CMAKE_SOURCE_DIR}/src/Programs/**/*.cpp")
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
    mongoc-1.0
    bson-1.0
    mongocxx
)