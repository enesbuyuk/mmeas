#pragma once
#include "config.h"

namespace vkInit {

    struct QueueFamilyIndices{
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() {return graphicsFamily.has_value() && presentFamily.has_value();}
    };

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

    QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface, bool debug){
        QueueFamilyIndices indices;

        std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

        if(debug) std::cout << "system can support " << queueFamilies.size() << " queue families.\n";

        int i = 0;

        for(const auto& queueFamily : queueFamilies){
            if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics){
                indices.graphicsFamily = i;
                indices.presentFamily = i;

                if (debug) std::cout << "queue family " << i << " is suitable for graphics and presenting." << "\n";
            }

            if(device.getSurfaceSupportKHR(i, surface)){
                indices.presentFamily = i;
                if (debug) std::cout << "queue family " << i << " is suitable for presenting.\n";
            }

            if(indices.isComplete()) break;
            i++;
        }

        return indices;
    }

    vk::Device create_logical_device(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, bool debug){
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface, debug);
        std::vector<uint32_t> uniqueIndices = {indices.graphicsFamily.value()};
        if (indices.graphicsFamily.value() != indices.presentFamily.value()){
            uniqueIndices.push_back(indices.presentFamily.value());
        }
        float queuePriority = 1.0f;
        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfo;
        for(const auto& queueFamilyIndex : uniqueIndices){
            queueCreateInfo.emplace_back(vk::DeviceQueueCreateFlags(), queueFamilyIndex, 1, &queuePriority);
        }

        vk::PhysicalDeviceFeatures deviceFeatures = vk::PhysicalDeviceFeatures();
        //deviceFeatures.samplerAnisotropy = true;

        std::vector <const char *> enabledLayers;
        if (debug) enabledLayers.push_back("VK_LAYER_KHRONOS_validation");


        vk::DeviceCreateInfo deviceInfo = vk::DeviceCreateInfo(
                vk::DeviceCreateFlags(),
                queueCreateInfo.size() , queueCreateInfo.data(),
                enabledLayers.size(), enabledLayers.data(),
                0, nullptr,
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

    std::array<vk::Queue,2> get_queue(vk::PhysicalDevice physicalDevice, vk::Device device, vk::SurfaceKHR surface, bool debug){
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface, debug);

        return { {
                device.getQueue(indices.graphicsFamily.value(), 0),
                device.getQueue(indices.presentFamily.value(), 0)
        }
        };
    }
}