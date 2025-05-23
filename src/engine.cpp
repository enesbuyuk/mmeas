#include "engine.h"
#include "instance.h"
#include "logging.h"
#include "device.h"
#include "swapchain.h"

Engine::Engine(bool debug) {
    debugMode = debug;
    if (debugMode) std::cout << "making a graphics engine" << std::endl;
    build_glfw_window();
    make_instance();
    make_device();
}

void Engine::build_glfw_window() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);


    if ((window=glfwCreateWindow(width, height, "vulkan", nullptr, nullptr))){
        if (debugMode) std::cout << "window created" << std::endl;
    } else {
        if (debugMode) std::cout << "window creation failed" << std::endl;
    }

}

void Engine::make_instance(){
    instance = vkInit::make_instance(debugMode, "MMEAS");
    dldi = vk::detail::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr);
    if (debugMode) debugMessenger = vkInit::make_debug_messenger(instance, dldi);
    VkSurfaceKHR c_style_surface;
    if (glfwCreateWindowSurface(instance, window, nullptr, &c_style_surface) != VK_SUCCESS){
        if (debugMode) std::cerr << "failed to abstract the glfw surface for vulkan\n";
    } else {
        if (debugMode) std::cout << "Successfully abstracted the glfw surface for vulkan.\n";
    }
    surface = c_style_surface;
}

void Engine::make_device(){
    physicalDevice = vkInit::choose_physical_device(instance, debugMode);
    device = vkInit::create_logical_device(physicalDevice, surface, debugMode);
    std::array<vk::Queue,2> queues = vkInit::get_queue(physicalDevice, device, surface, debugMode);
    graphicsQueue = queues[0];
    presentQueue = queues[1];
    vkInit::SwapChainBundle bundle = vkInit::create_swapchain(device, physicalDevice, surface, width, height, debugMode);
    swapchain = bundle.swapchain;
    swapchainFrames = bundle.frames;
    swapchainFormat = bundle.format;
    swapchainExtent = bundle.extent;
}

Engine::~Engine(){
    if (debugMode)
    std::cout << "destroying graphics engine" << std::endl;

    for (auto& frame : swapchainFrames){
        device.destroyImageView(frame.imageView);
    }

    device.destroySwapchainKHR(swapchain);
    device.destroy();

    instance.destroySurfaceKHR(surface);
    if (debugMode){
        instance.destroyDebugUtilsMessengerEXT(debugMessenger, nullptr, dldi);
    }

    instance.destroy();

    glfwTerminate();
}