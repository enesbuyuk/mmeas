#pragma once
#include "config.h"
#include "vkUtil/QueueFamilies.h"

namespace vkInit {
    bool CheckDeviceExtensionSupport(const vk::PhysicalDevice& device, const std::vector<const char*>& requestedExtensions, bool debug){
        std::set<std::string> requiredExtensions(requestedExtensions.begin(), requestedExtensions.end());

        if (debug) std::cout << "device can support extensions:" << "\n";
        for (vk::ExtensionProperties& extension : device.enumerateDeviceExtensionProperties()){
            if (debug) std::cout << "\t\"" << extension.extensionName << "\"\n";
            requiredExtensions.erase(extension.extensionName);
        }
        return requiredExtensions.empty();
    }

    bool IsSuitable(const vk::PhysicalDevice& device, const bool debug) {
        if (debug) std::cout << "checking if device is suitable" << "\n";

        const std::vector<const char *> requestedExtensions = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        if (debug) {
            std::cout << "requested extensions: " << std::endl;
            for (const char *ext: requestedExtensions) std::cout << "\t\"" << ext << "\"\n";
        }

        if (bool extensionSupported = CheckDeviceExtensionSupport(device, requestedExtensions, debug)){
            if (debug) std::cout << "device supports all requested extensions" << "\n";
        } else {
            if (debug) std::cerr << "device does not support all requested extensions" << "\n";
            return false;
        }
        return true;
    }

    vk::PhysicalDevice ChoosePhysicalDevice(vk::Instance& instance, bool debug){
        if (debug) std::cout<< "choosing physical device" << "\n";

        std::vector<vk::PhysicalDevice> devices = instance.enumeratePhysicalDevices();

        if (debug) std::cout << "There are " << devices.size() << " physical devices available on this system\n";

        for (vk::PhysicalDevice device : devices){
            //std::cout << "device name: " << device.getProperties().deviceName << "\n";
            if (debug) LogDeviceProperties(device);
            if (IsSuitable(device, debug)) return device;
        }

        return nullptr;
    }

    vk::Device CreateLogicalDevice(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, bool debug){
        vkUtil::QueueFamilyIndices indices = vkUtil::FindQueueFamilies(physicalDevice, surface, debug);
        std::vector<uint32_t> uniqueIndices = {indices.graphicsFamily.value()};
        if (indices.graphicsFamily.value() != indices.presentFamily.value()){
            uniqueIndices.push_back(indices.presentFamily.value());
        }
        float queuePriority = 1.0f;
        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfo;
        for(const auto& queueFamilyIndex : uniqueIndices){
            queueCreateInfo.emplace_back(vk::DeviceQueueCreateFlags(), queueFamilyIndex, 1, &queuePriority);
        }

        std::vector<const char*> deviceExtensions = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        vk::PhysicalDeviceFeatures deviceFeatures = vk::PhysicalDeviceFeatures();
        //deviceFeatures.samplerAnisotropy = true;

        std::vector <const char *> enabledLayers;
        if (debug) enabledLayers.push_back("VK_LAYER_KHRONOS_validation");


        vk::DeviceCreateInfo deviceInfo = vk::DeviceCreateInfo(
                vk::DeviceCreateFlags(),
                queueCreateInfo.size() , queueCreateInfo.data(),
                enabledLayers.size(), enabledLayers.data(),
                deviceExtensions.size(), deviceExtensions.data(),
                &deviceFeatures
                );
        try{
            vk::Device device = physicalDevice.createDevice(deviceInfo);
            if (debug) std::cout << "gpu has been successfully abstracted" << "\n";
            return device;
        }catch(vk::SystemError err){
            if (debug) std::cerr << "failed to create logical device: " << err.what() << "\n";
            return nullptr;
        }
        return nullptr;
    }

    std::array<vk::Queue,2> GetQueue(vk::PhysicalDevice physicalDevice, vk::Device device, vk::SurfaceKHR surface, bool debug){
        vkUtil::QueueFamilyIndices indices = vkUtil::FindQueueFamilies(physicalDevice, surface, debug);

        return { {
                device.getQueue(indices.graphicsFamily.value(), 0),
                device.getQueue(indices.presentFamily.value(), 0)
        }
        };
    }
}