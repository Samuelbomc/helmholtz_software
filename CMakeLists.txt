cmake_minimum_required(VERSION 3.30.5)

project(helmholtz LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/data")

set(GMAT_ZIP "${CMAKE_SOURCE_DIR}/data/gmat-latest.zip")
set(GMAT_EXTRACT_DIR "${CMAKE_SOURCE_DIR}/data/gmat-extracted")

# Check if the GMAT archive already exists, if not, download it
if(NOT EXISTS "${GMAT_ZIP}")
    file(DOWNLOAD "https://sourceforge.net/projects/gmat/files/latest/download"
         "${GMAT_ZIP}"
         STATUS download_status
         LOG download_log
         SHOW_PROGRESS)

    if(NOT download_status EQUAL 0)
        message(FATAL_ERROR "Download failed: ${download_log}")
    endif()
else()
    message(STATUS "GMAT archive already exists at ${GMAT_ZIP}, skipping download.")
endif()

# Extract the ZIP if not already extracted
if(NOT EXISTS "${GMAT_EXTRACT_DIR}")
    file(MAKE_DIRECTORY "${GMAT_EXTRACT_DIR}")
    file(ARCHIVE_EXTRACT
        INPUT "${GMAT_ZIP}"
        DESTINATION "${GMAT_EXTRACT_DIR}"
    )
endif()

# Find the top-level directory inside the extracted folder
file(GLOB GMAT_EXTRACTED_DIRS RELATIVE "${GMAT_EXTRACT_DIR}" "${GMAT_EXTRACT_DIR}/*")
list(GET GMAT_EXTRACTED_DIRS 0 GMAT_TOPLEVEL_DIR)
set(GMAT_TOPLEVEL_PATH "${GMAT_EXTRACT_DIR}/${GMAT_TOPLEVEL_DIR}")

configure_file(
    "${CMAKE_SOURCE_DIR}/GMATConfig.h.in"
    "${CMAKE_BINARY_DIR}/GMATConfig.h"
    @ONLY
)

add_executable(main
    main.cpp
)

add_library(GMATRunner
    "GMAT/GMATRunner.cpp"
    "GMAT/GMATRunner.h"
)

add_library(GMATScripter
    "GMAT/GMATScripter.cpp"
    "GMAT/GMATScripter.h"
)

add_library(ArduinoConfig
    "Hardware/ArduinoConfig.cpp"
    "Hardware/ArduinoConfig.h"
)

add_library(PWRSRCConfig
    "Hardware/PWRSRCConfig.cpp"
    "Hardware/PWRSRCConfig.h"
)

add_library(Simulation
    "Sim/Simulation.cpp"
    "Sim/Simulation.h"
)

add_library(Visualization
    "UI/Visualization.cpp"
    "UI/Visualization.h"
)

target_include_directories(main PRIVATE "${CMAKE_BINARY_DIR}")
target_include_directories(GMATRunner PRIVATE "${CMAKE_BINARY_DIR}")
target_include_directories(GMATScripter PRIVATE "${CMAKE_BINARY_DIR}")
target_include_directories(ArduinoConfig PRIVATE "${CMAKE_BINARY_DIR}")
target_include_directories(PWRSRCConfig PRIVATE "${CMAKE_BINARY_DIR}")
target_include_directories(Simulation PRIVATE "${CMAKE_BINARY_DIR}")

target_link_libraries(Simulation PRIVATE GMATRunner GMATScripter ArduinoConfig PWRSRCConfig)
target_link_libraries(main PRIVATE Visualization)
target_link_libraries(Visualization PRIVATE )
