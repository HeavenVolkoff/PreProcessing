cmake_minimum_required(VERSION 3.5)
project(PreProcessing)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

find_package(FFTW)
find_package(LibSndFile)
find_package(LibPortAudio)

if (NOT PORTAUDIO_FOUND)
    message(FATAL_ERROR "PortAudio not found")

elseif(NOT LIBSNDFILE_FOUND)
    message(FATAL_ERROR "LibSndFile not found")

elseif(NOT FFTW_FOUND)
    message(FATAL_ERROR "FFTW not found")

endif()

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

set(Flags "-std=c++14")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${Flags} ${PORTAUDIO_DEFINITIONS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Flags} ${PORTAUDIO_DEFINITIONS}")

set(SOURCE_FILES main.cpp)
add_executable(PreProcessing ${SOURCE_FILES})
include_directories(${PORTAUDIO_INCLUDE_DIRS} ${LIBSNDFILE_INCLUDE_DIRS} ${FFTW_INCLUDES})
target_link_libraries(${PROJECT_NAME} ${PORTAUDIO_LIBRARIES} ${LIBSNDFILE_LIBRARIES} ${FFTW_LIBRARIES})