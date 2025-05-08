#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include "config.h"

class Instance;

class Engine{
public:
    Engine();
    ~Engine();
private:
    bool debugMode = true;

    int width{800}, height{600};
    GLFWwindow* window{nullptr};

    void build_glfw_window();

    //vulkan instance
    vk::Instance instance{nullptr};

    void make_instance();

};
