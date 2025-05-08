#pragma once
#include "config.h"

namespace vkInit{
    vk::Instance make_instance(bool debug, const char* appName){
        if (debug) std::cout << "making an instance" << std::endl;

        uint32_t version{0};
        vkEnumerateInstanceVersion(&version);
        if (debug){
            std::cout << "Vulkan version: " << VK_VERSION_MAJOR(version) << "."
                      << VK_VERSION_MINOR(version) << "."
                      << VK_VERSION_PATCH(version) << "."
                      << VK_VERSION_PATCH(version) << std::endl;
        }
        return nullptr;
    }
}
