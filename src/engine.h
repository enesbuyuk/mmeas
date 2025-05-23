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


    // instance-related
    vk::Instance instance{nullptr};
    vk::DebugUtilsMessengerEXT debugMessenger{nullptr};
    vk::detail::DispatchLoaderDynamic dldi;
    vk::SurfaceKHR surface;

    // device
    vk::PhysicalDevice physicalDevice{nullptr};
    vk::Device device{nullptr};
    vk::Queue graphicsQueue{nullptr};
    vk::Queue presentQueue{nullptr};
    vk::SwapchainKHR swapchain;
    std::vector<vk::Image> swapchainImages;
    vk::Format swapchainFormat;
    vk::Extent2D swapchainExtent;

    void build_glfw_window();

    void make_instance();


    void make_device();
};
