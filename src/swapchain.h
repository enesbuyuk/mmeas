#pragma once
#include "config.h"
#include "logging.h"
#include "QueueFamilies.h"
#include "frame.h"

namespace vkInit {
    struct SwapChainSupportDetails{
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;

        bool IsComplete() {return !formats.empty() && !presentModes.empty();}
    };

    struct SwapChainBundle{
        vk::SwapchainKHR swapchain;
        std::vector<vkUtil::SwapChainFrame> frames;
        vk::Format format;
        vk::Extent2D extent;
    };

    SwapChainSupportDetails QuerySwapchainSupport(vk::PhysicalDevice device, vk::SurfaceKHR surface, bool debug){
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
            stringList = LogAlphaCompositeBits(support.capabilities.supportedCompositeAlpha);
            for (const auto& str: stringList) std::cout << "\t\t" << str << "\n";

            std::cout << "\tsupported image usage: \n";
            stringList = LogImageUsageBits(support.capabilities.supportedUsageFlags);
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
                std::cout << "\tsupported present mode: " << LogPresentMode(presentMode) << "\n";
            }
        }

        return support;
    }

    vk::SurfaceFormatKHR ChooseSwapchainSurfaceFormat(std::vector<vk::SurfaceFormatKHR> formats){
        for(const auto& format : formats){
            if(format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear){
                return format;
            }
        }
        return formats[0];
    }

    vk::PresentModeKHR ChooseSwapchainPresentMode(std::vector<vk::PresentModeKHR> presentModes){
        for (const auto& presentMode : presentModes){
            if (presentMode == vk::PresentModeKHR::eMailbox){
                return presentMode;
            }
        }
        return vk::PresentModeKHR::eFifo;
    }

    vk::Extent2D ChooseSwapchainExtent(uint32_t width, uint32_t height, vk::SurfaceCapabilitiesKHR capabilities){
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

    SwapChainBundle CreateSwapchain(vk::Device logicalDevice, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, int width, int height, bool debug){
        SwapChainSupportDetails support = QuerySwapchainSupport(physicalDevice, surface, debug);
        vk::SurfaceFormatKHR format = ChooseSwapchainSurfaceFormat(support.formats);
        vk::PresentModeKHR presentMode = ChooseSwapchainPresentMode(support.presentModes);
        vk::Extent2D extent = ChooseSwapchainExtent(width, height, support.capabilities);
        uint32_t imageCount = std::min(
                support.capabilities.maxImageCount,
                support.capabilities.minImageCount + 1
        );
        vk::SwapchainCreateInfoKHR createInfo = vk::SwapchainCreateInfoKHR(
                vk::SwapchainCreateFlagsKHR(),
                surface, imageCount, format.format, format.colorSpace,
                extent, 1 , vk::ImageUsageFlagBits::eColorAttachment
        );
        vkUtil::QueueFamilyIndices indices = vkUtil::FindQueueFamilies(physicalDevice, surface, debug);
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

        std::vector<vk::Image> images = logicalDevice.getSwapchainImagesKHR(bundle.swapchain);
        bundle.frames.resize(images.size());

        for (size_t i = 0; i<images.size();i++){
            vk::ImageViewCreateInfo createInfo = {};
            createInfo.image = images[i];
            createInfo.viewType = vk::ImageViewType::e2D;
            createInfo.components.r = vk::ComponentSwizzle::eIdentity;
            createInfo.components.g = vk::ComponentSwizzle::eIdentity;
            createInfo.components.b = vk::ComponentSwizzle::eIdentity;
            createInfo.components.a = vk::ComponentSwizzle::eIdentity;
            createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;
            createInfo.format = format.format;

            bundle.frames[i].image = images[i];
            bundle.frames[i].imageView = logicalDevice.createImageView(createInfo);
        }

        bundle.format = format.format;
        bundle.extent = extent;

        return bundle;

    }
}