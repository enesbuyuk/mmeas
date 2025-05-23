#pragma once
#include "config.h"

namespace vkInit {

    struct QueueFamilyIndices{
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() {return graphicsFamily.has_value() && presentFamily.has_value();}
    };

    struct SwapChainSupportDetails{
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;

        bool isComplete() {return !formats.empty() && !presentModes.empty();}
    };

    struct SwapChainBundle{
        vk::SwapchainKHR swapchain;
        std::vector<vk::Image> images;
        vk::Format format;
        vk::Extent2D extent;

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

    std::array<vk::Queue,2> get_queue(vk::PhysicalDevice physicalDevice, vk::Device device, vk::SurfaceKHR surface, bool debug){
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface, debug);

        return { {
                device.getQueue(indices.graphicsFamily.value(), 0),
                device.getQueue(indices.presentFamily.value(), 0)
        }
        };
    }

    SwapChainSupportDetails query_swapchain_support(vk::PhysicalDevice device, vk::SurfaceKHR surface, bool debug){
        SwapChainSupportDetails support;
        support.capabilities = device.getSurfaceCapabilitiesKHR(surface);
        if (debug){
            std::cout << "swapchain can support the following surface capabilities:\n";
            std::cout << "\tminImageCount: " << support.capabilities.minImageCount << "\n";
            std::cout << "\tmaxImageCount: " << support.capabilities.maxImageCount << "\n";

//            vk::Extent2D extent{120,234};
            std::cout << "\tcurrentExtent: \n";
            std::cout << "\t\twidth: " << support.capabilities.currentExtent.width << "\n";
            std::cout << "\t\theight: " << support.capabilities.currentExtent.height << "\n";

            std::cout << "\tmin supported extent: \n";
            std::cout << "\t\twidth: " << support.capabilities.minImageExtent.width << "\n";
            std::cout << "\t\theight: " << support.capabilities.minImageExtent.height << "\n";

            std::cout << "\tmax supported extent: \n";
            std::cout << "\t\twidth: " << support.capabilities.maxImageExtent.width << "\n";
            std::cout << "\t\theight: " << support.capabilities.maxImageExtent.height << "\n";

            std::cout << "\tmax image array layers: " << support.capabilities.maxImageArrayLayers << "\n";


            std::cout << "\tCurrent transform: \n";
            std::vector<std::string> stringList = log_transform_bits(support.capabilities.currentTransform);
            for (const auto& str: stringList) std::cout << "\t\t" << str << "\n";

            std::cout << "\tsupported transform: \n";
            stringList = log_transform_bits(support.capabilities.supportedTransforms);
            for (const auto& str: stringList) std::cout << "\t\t" << str << "\n";

            std::cout << "\tsupported alpha operations: \n";
            stringList = log_alpha_composite_bits(support.capabilities.supportedCompositeAlpha);
            for (const auto& str: stringList) std::cout << "\t\t" << str << "\n";

            std::cout << "\tsupported image usage: \n";
            stringList = log_image_usage_bits(support.capabilities.supportedUsageFlags);
            for (const auto& str: stringList) std::cout << "\t\t" << str << "\n";

        }
//
        support.formats = device.getSurfaceFormatsKHR(surface);
        if(debug){
            for (const auto& supported : support.formats) {
                std::cout << "\tsupported pixel format: " << vk::to_string(supported.format) << "\n";
                std::cout << "\tcolor space: " << vk::to_string(supported.colorSpace) << "\n";
            }
        }

        support.presentModes = device.getSurfacePresentModesKHR(surface);
        if(debug){
            for (const auto& presentMode : support.presentModes) {
                std::cout << "\tsupported present mode: " << log_present_mode(presentMode) << "\n";
            }
        }

        return support;
    }

    vk::SurfaceFormatKHR choose_swapchain_surface_format(std::vector<vk::SurfaceFormatKHR> formats){
        for(const auto& format : formats){
            if(format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear){
                return format;
            }
        }
        return formats[0];
    }

    vk::PresentModeKHR choose_swapchain_present_mode(std::vector<vk::PresentModeKHR> presentModes){
        for (const auto& presentMode : presentModes){
            if (presentMode == vk::PresentModeKHR::eMailbox){
                return presentMode;
            }
        }
        return vk::PresentModeKHR::eFifo;
    }

    vk::Extent2D choose_swapchain_extent(uint32_t width, uint32_t height, vk::SurfaceCapabilitiesKHR capabilities){
        if(capabilities.currentExtent.width != UINT32_MAX){
            return capabilities.currentExtent;
        }else{
            vk::Extent2D extent = {width, height};
            extent.width = std::min(
                    capabilities.maxImageExtent.width,
                    std::max(capabilities.minImageExtent.width, width)
                    );
            extent.height = std::min(
                    capabilities.maxImageExtent.height,
                    std::max(capabilities.minImageExtent.height, height)
                    );
            return extent;
        }
    }

    SwapChainBundle create_swapchain(vk::Device logicalDevice, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, int width, int height, bool debug){
        SwapChainSupportDetails support = query_swapchain_support(physicalDevice, surface, debug);
        vk::SurfaceFormatKHR surfaceFormat = choose_swapchain_surface_format(support.formats);
        vk::PresentModeKHR presentMode = choose_swapchain_present_mode(support.presentModes);
        vk::Extent2D extent = choose_swapchain_extent(width, height, support.capabilities);
        uint32_t imageCount = std::min(
                support.capabilities.maxImageCount,
                support.capabilities.minImageCount + 1
                );
        vk::SwapchainCreateInfoKHR createInfo = vk::SwapchainCreateInfoKHR(
                vk::SwapchainCreateFlagsKHR(),
                surface, imageCount, surfaceFormat.format, surfaceFormat.colorSpace,
                extent, 1 , vk::ImageUsageFlagBits::eColorAttachment
                );
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface, debug);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        if (indices.graphicsFamily.value() != indices.presentFamily.value()){
            createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
            createInfo.queueFamilyIndexCount = 2; // two queue families
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }else{
            createInfo.imageSharingMode = vk::SharingMode::eExclusive;
        }

        createInfo.preTransform = support.capabilities.currentTransform;
        createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        createInfo.oldSwapchain = vk::SwapchainKHR(nullptr);

        SwapChainBundle bundle{};
        try{
            bundle.swapchain = logicalDevice.createSwapchainKHR(createInfo);
        }catch(vk::SystemError err){
            throw std::runtime_error("failed to create swapchain: " + std::string(err.what()));
        }

        bundle.images = logicalDevice.getSwapchainImagesKHR(bundle.swapchain);
        bundle.format = surfaceFormat.format;
        bundle.extent = extent;

        return bundle;

    }
}

