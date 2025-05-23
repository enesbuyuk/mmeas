cmake_minimum_required(VERSION 3.27)
project(mmeas)

include(FetchContent)
FETCHCONTENT_DECLARE(GLFW GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.4)
FETCHCONTENT_MAKEAVAILABLE(GLFW)

find_package(Vulkan REQUIRED)
if (Vulkan_FOUND)
    include_directories(${Vulkan_INCLUDE_DIRS})
    link_directories(${Vulkan_LIBRARY_DIRS})
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(mmeas src/main.cpp
        src/engine.cpp
        src/engine.h
        src/engine.h
        src/instance.h
        src/config.h
        src/logging.h
        src/device.h
        src/swapchain.h
        src/QueueFamilies.h
        src/frame.h)

target_link_libraries(mmeas glfw ${Vulkan_LIBRARIES})