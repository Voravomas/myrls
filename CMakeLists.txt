cmake_minimum_required(VERSION 3.15)
project(myrls
        LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


# Options
option(WARNINGS_AS_ERRORS   "Treat compiler warnings as errors."    OFF)
option(ENABLE_CONAN         "Use Conan as a package manager."       OFF)

# ENABLE_PVS_STUDIO is the option for local usage only. PVS-Studio isn't installed on GitHub machines.
option(ENABLE_PVS_STUDIO    "Use PVS-Studio static code analyzer."  OFF)

# ENABLE_SANITIZERS is the option for the test builds. Do not use it in the production builds.
option(ENABLE_SANITIZERS    "Use sanitizers to detect errors."      OFF)


# Include default CMake configuration
include(cmake/defaults.cmake)


# Project source compilation
add_executable(myrls src/main.cpp header/file_manip.h src/file_manip.cpp header/minor.h src/minor.cpp header/printers.h src/printers.cpp header/structs.h header/primary.h src/primary.cpp)


# Add external packages
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
target_link_libraries(myrls Boost::program_options)


# PVS-Studio configuration
if (ENABLE_PVS_STUDIO)
    # CMake targets to which apply PVS-Studio
    pvs_studio_add_target(TARGET ${PROJECT_NAME}.analyze ALL
                          OUTPUT FORMAT errorfile
                          ANALYZE arithmetic add
                          LOG target.err)
endif ()


# Sanitizers configuration
if (ENABLE_SANITIZERS)
    # CMake targets to which apply sanitizers
    add_sanitizers(arithmetic add)
endif ()
