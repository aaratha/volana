cmake_minimum_required(VERSION 3.25)
project(volana VERSION 0.1 LANGUAGES CXX)

# -----------------------------
# C++ standard
# -----------------------------
set(CMAKE_CXX_STANDARD 17)          # or 20 if you want
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# -----------------------------
# Source files
# -----------------------------
file(GLOB_RECURSE PROJECT_SRC "${PROJECT_SOURCE_DIR}/src/*.cpp")

set(EXTERNAL_SRC
    "${PROJECT_SOURCE_DIR}/external/miniaudio_wrapper.cpp"
)

set(ALL_SOURCES ${PROJECT_SRC} ${EXTERNAL_SRC})

# -----------------------------
# Add executable
# -----------------------------
add_executable(${PROJECT_NAME} ${ALL_SOURCES})

# -----------------------------
# Include directories
# -----------------------------
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/external/miniaudio
)

# -----------------------------
# Optional: link libraries
# -----------------------------
# target_link_libraries(${PROJECT_NAME} PRIVATE some_library)

