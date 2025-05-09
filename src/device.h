#pragma once
#include "config.h"

namespace vkInit {
    void log_device_properties(const vk::PhysicalDevice& device){
        vk::PhysicalDeviceProperties properties = device.getProperties();
        std::cout << "device name: " << properties.deviceName << "\n";
        std::cout << "device type : ";
        switch (properties.deviceType) {
            case vk::PhysicalDeviceType::eOther:
                std::cout << "other\n";
                break;
            case vk::PhysicalDeviceType::eIntegratedGpu:
                std::cout << "integrated gpu\n";
                break;
            case vk::PhysicalDeviceType::eDiscreteGpu:
                std::cout << "discrete gpu\n";
                break;
            case vk::PhysicalDeviceType::eVirtualGpu:
                std::cout << "virtual gpu\n";
                break;
            case vk::PhysicalDeviceType::eCpu:
                std::cout << "cpu\n";
                break;
            default:
                std::cout << "other\n";
        }
    }
    bool checkDeviceExtensionSupport(
            const vk::PhysicalDevice& device,
            const std::vector<const char*>& requestedExtensions,
            bool debug
            ){
        std::set<std::string> requiredExtensions(requestedExtensions.begin(), requestedExtensions.end());

        if (debug) std::cout << "device can support extensions:" << "\n";
        for (vk::ExtensionProperties& extension : device.enumerateDeviceExtensionProperties()){
            if (debug) std::cout << "\t\"" << extension.extensionName << "\"\n";
            requiredExtensions.erase(extension.extensionName);
        }
        return requiredExtensions.empty();
    }
    bool isSuitable(const vk::PhysicalDevice& device, const bool debug) {
        if (debug) std::cout << "checking if device is suitable" << "\n";

        const std::vector<const char *> requestedExtensions = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        if (debug) {
            std::cout << "requested extensions: " << std::endl;
            for (const char *ext: requestedExtensions) std::cout << "\t\"" << ext << "\"\n";
        }

        if (bool extensionSupported = checkDeviceExtensionSupport(device, requestedExtensions, debug)){
            if (debug) std::cout << "device supports all requested extensions" << "\n";
        } else {
            if (debug) std::cerr << "device does not support all requested extensions" << "\n";
            return false;
        }
        return true;
    }
    vk::PhysicalDevice choose_physical_device(vk::Instance& instance, bool debug){
        if (debug) std::cout<< "choosing physical device" << "\n";

        std::vector<vk::PhysicalDevice> devices = instance.enumeratePhysicalDevices();

        if (debug) std::cout << "There are " << devices.size() << " physical devices available on this system\n";

        for (vk::PhysicalDevice device : devices){
            //std::cout << "device name: " << device.getProperties().deviceName << "\n";
            if (debug) log_device_properties(device);
            if (isSuitable(device, debug)) return device;
        }

        return nullptr;
    }
}