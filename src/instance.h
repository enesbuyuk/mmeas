#pragma once
#include "config.h"

namespace vkInit{
    bool supported(std::vector<const char*>& extensions, std::vector<const char*>& layers, bool debug){
        std::vector<vk::ExtensionProperties> supportedExtensions = vk::enumerateInstanceExtensionProperties();

        if (debug){
            std::cout << "device can support the following extensions: " << std::endl;
            for (vk::ExtensionProperties ext : supportedExtensions){
                std::cout << "\t\"" << ext.extensionName << "\"\n";
            }
        }

        bool extensionFound = false;
        std::vector<const char*> notFoundExtensions;

        for (const char* extensionName : extensions){
            for (vk::ExtensionProperties supportedExtension : supportedExtensions ){
                if (strcmp(extensionName, supportedExtension.extensionName) == 0 ){
                    notFoundExtensions.push_back(extensionName);
                    extensionFound = true;
                    if (debug) std::cout << "extension " << extensionName << " is supported" << "\n";
                }
            }
        }

        // check layer support
        std::vector<vk::LayerProperties> supportedLayers = vk::enumerateInstanceLayerProperties();
        if (debug){
            std::cout << "device can support the following layers: " << "\n";
            for (vk::LayerProperties layer : supportedLayers){
                std::cout << "\t\"" << layer.layerName << "\"\n";
            }
        }

        bool layerFound = false;
        std::vector<const char*> notFoundLayers;

        for (const char* layerName : layers){
            for (vk::LayerProperties supportedLayer : supportedLayers ){
                if (strcmp(layerName, supportedLayer.layerName) == 0 ){
                    notFoundLayers.push_back(layerName);
                    layerFound = true;
                    if (debug) std::cout << "layer " << layerName << " is supported" << "\n";
                }
            }
        }

        if (!extensionFound){
            if (debug) std::cerr << "failed to find required extensions" << std::endl;
            for (const char* ext : notFoundExtensions){
                std::cerr << "\t\"" << ext << "\"\n";
            }
        }

        if (!layerFound){
            if (debug) std::cerr << "failed to find required layers" << std::endl;
            for (const char* layer : layers){
                std::cerr << "\t\"" << layer << "\"\n";
            }
        }

        if (!extensionFound || !layerFound) return false;

        return true;
    }

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
        if (debug) {
            extensions.push_back("VK_EXT_debug_utils");
        }

        /*
         *
         * VK_KHR_surface VK_KHR_win32_surface extension
         */
        if (debug){
            std::cout << "glfw extensions: " << std::endl;
            for (const char* extName : extensions) std::cout << "\t\"" << extName << "\"\n" ;
        }

        std::vector<const char*> layers;
        if (debug) {
            layers.push_back("VK_LAYER_KHRONOS_validation");
        }

        if (!supported(extensions, layers, debug)){
            std::cerr << "failed to find required extensions" << std::endl;
            return nullptr;
        }

        /*
         * from vulkan_structs.hpp
         *
         */

        vk::InstanceCreateInfo createInfo = vk::InstanceCreateInfo(
                vk::InstanceCreateFlags(),
                &appInfo,
                static_cast<uint32_t>(layers.size()), layers.data(),
                static_cast<uint32_t>(extensions.size()),extensions.data());

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
