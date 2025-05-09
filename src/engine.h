#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include "config.h"

class Instance;

class Engine{
public:
    Engine(bool debug);
    ~Engine();
private:
    bool debugMode = true;

    int width{800}, height{600};
    GLFWwindow* window{nullptr};


    //vulkan instance
    vk::Instance instance{nullptr};

    // debug callback
    vk::DebugUtilsMessengerEXT debugMessenger{nullptr};

    // dynamic instance dispatcher
    vk::detail::DispatchLoaderDynamic dldi;

    void build_glfw_window();


    void make_instance();

    // debug messenger
    void make_debug_messenger();

};
