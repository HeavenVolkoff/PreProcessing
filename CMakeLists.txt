cmake_minimum_required(VERSION 3.5)

# Project Name (and also the compiled binary's name)
project(PreProcessing)
set(CMAKE_BUILD_TYPE Release)

# Flags to send to the compiler
# OBS: Do NOT use Linker OR Include flags (CMAKE take care of this for you)
set(Flags "-std=c++14 -O3")

# List of Header files (.h, .hh, .hpp)
set(HEADER_FILES
        include/PreProcessing.h
        )

# List of Source files (.c, .cc, .cpp)
set(SOURCE_FILES
        src/PreProcessing.cpp
        )

# Include Projet cmake scripts (Mostly used to find dependencies libraries on the system)
set(CMAKE_MODULE_PATH
        ${CMAKE_MODULE_PATH}
        ${CMAKE_SOURCE_DIR}/cmake
        )

# Execute each dependency find_cmake script
find_package(FFTW REQUIRED)
find_package(LibSndFile REQUIRED)
# find_package(LibPortAudio REQUIRED)

# Same as above, but OpenMP isn't a obligatory dependency
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# Set the compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${Flags} ${PORTAUDIO_DEFINITIONS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Flags} ${PORTAUDIO_DEFINITIONS}")

# Specify which project's files will be compiled
add_library(PreProcessing SHARED
        ${HEADER_FILES}
        ${SOURCE_FILES}
        )

# Include the dependencies header files to be compiled
include_directories(
        ${FFTW_INCLUDES}
        lib/spdlog/include
        ${PORTAUDIO_INCLUDE_DIRS}
        ${LIBSNDFILE_INCLUDE_DIRS}
        )

# Link the dependencies libs
target_link_libraries(
        ${PROJECT_NAME}
        ${FFTW_LIBRARIES}
        ${PORTAUDIO_LIBRARIES}
        ${LIBSNDFILE_LIBRARIES}
        )
