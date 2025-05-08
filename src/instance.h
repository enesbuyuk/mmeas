#pragma once
#include "config.h"

namespace vkInit{
    vk::Instance make_instance(bool debug, const char* appName){
        if (debug) std::cout << "making an instance" << std::endl;

        uint32_t version{0};
        vkEnumerateInstanceVersion(&version);

        version &= ~(0xFFFU);


        if (debug){
            std::cout << "Vulkan version: " << VK_API_VERSION_MAJOR(version) << "."
                      << VK_API_VERSION_MINOR(version) << "."
                      << VK_API_VERSION_PATCH(version) << "."
                      << VK_API_VERSION_PATCH(version) << std::endl;
        }
        version = VK_MAKE_API_VERSION(0, 1, 0, 0);
        vk::ApplicationInfo appInfo = vk::ApplicationInfo(
                appName,
                version,
                "MMEAS",
                version,
                version
        );

        uint32_t glfwExtensionCount{0};
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);


        /*
         *
         * VK_KHR_surface VK_KHR_win32_surface extension
         */
        if (debug){
            std::cout << "glfw extensions: " << std::endl;
            for (const char* extName : extensions) std::cout << "\t\"" << extName << "\"\n" << std::endl;
        }

        /*
         * from vulkan_structs.hpp
         *
         */

        vk::InstanceCreateInfo createInfo = vk::InstanceCreateInfo(
                vk::InstanceCreateFlags(),
                &appInfo,
                0,
                nullptr,
                static_cast<uint32_t>(extensions.size()),
                extensions.data());

        try {
            /*
             * Declared in: vulkan_funcs.hpp
             */
            return vk::createInstance(createInfo);
        }catch(const vk::SystemError& error){
            std::cerr << "failed to create instance: " << error.what() << std::endl;
            return nullptr;
        }
    }
}
