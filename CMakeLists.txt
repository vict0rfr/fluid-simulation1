﻿cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Download and install vcpkg
include(ExternalProject)
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/vcpkg")
    execute_process(
        COMMAND git clone https://github.com/Microsoft/vcpkg.git
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    if(WIN32)
        execute_process(
            COMMAND .\\vcpkg\\bootstrap-vcpkg.bat
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
    else()
        execute_process(
            COMMAND ./vcpkg/bootstrap-vcpkg.sh
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
    endif()
endif()

# Install packages based on platform
if(APPLE)
    execute_process(
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/vcpkg install sdl3:arm64-osx imgui:arm64-osx
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
elseif(WIN32)
    execute_process(
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/vcpkg install sdl3:x64-windows imgui:x64-windows
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
else()
    execute_process(
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/vcpkg install sdl3:x64-linux imgui:x64-linux
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

project ("FluidSimulation")
set(CMAKE_CXX_STANDARD 20)

# Platform-specific settings
if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
endif()

file(GLOB_RECURSE SOURCES "src/*.cpp")

# Add executable with conditional resource file
if(WIN32)
    add_executable(${PROJECT_NAME} ${SOURCES} res/resource.rc)
else()
    add_executable(${PROJECT_NAME} ${SOURCES})
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()

# Add vcpkg toolchain file
include("vcpkg/scripts/buildsystems/vcpkg.cmake")

target_include_directories(${PROJECT_NAME} PRIVATE "include")

#add_definitions(-DIMGUI_IMPL_SDL_RENDERER)

find_package(SDL3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 imgui::imgui)

# FetchContent for ImGui
include(FetchContent)
FetchContent_Declare(imgui GIT_REPOSITORY https://github.com/ocornut/imgui.git GIT_TAG v1.91.6) # Make sure to use the same version that vcpkg installs
FetchContent_MakeAvailable(imgui)

# Add include directories _deps
target_include_directories(${PROJECT_NAME} PRIVATE ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)

# Specify the paths to the ImGui backend files
set(IMGUI_BACKEND_SOURCES
    "${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp"
)

# Add the ImGui backend files to the project
target_sources(${PROJECT_NAME} PRIVATE ${IMGUI_BACKEND_SOURCES})

# Copy DLLs only on Windows
if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_FILE:SDL3::SDL3>
      $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()