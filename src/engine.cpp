#include "engine.h"
#include "instance.h"
#include "logging.h"

Engine::Engine() {
    if (debugMode) std::cout << "making a graphics engine" << std::endl;

    build_glfw_window();

    make_instance();

    make_debug_messenger();
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
}

void Engine::make_debug_messenger(){
    debugMessenger = vkInit::make_debug_messenger(instance, dldi);
}

Engine::~Engine(){
    std::cout << "destroying graphics engine" << std::endl;

    instance.destroyDebugUtilsMessengerEXT(debugMessenger, nullptr, dldi);

    instance.destroy();

    glfwTerminate();
}