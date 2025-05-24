#pragma once

#include <vulkan/vulkan.hpp>
#include <iostream>

namespace vkInit {
    VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallBack(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
            void *pUserData
    ) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    }

    vk::DebugUtilsMessengerEXT MakeDebugMessenger(vk::Instance &instance, vk::detail::DispatchLoaderDynamic &dldi) {
        vk::DebugUtilsMessengerCreateInfoEXT createInfo = vk::DebugUtilsMessengerCreateInfoEXT(
                vk::DebugUtilsMessengerCreateFlagsEXT(),
                // vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
                vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
                DebugCallBack,
                nullptr
        );

        return instance.createDebugUtilsMessengerEXT(
                createInfo,
                nullptr,
                dldi
        );
    };

    std::vector<std::string> log_transform_bits(vk::Flags<vk::SurfaceTransformFlagBitsKHR> bits) {
        std::vector<std::string> result;
        if (bits & vk::SurfaceTransformFlagBitsKHR::eIdentity) result.emplace_back("identity");
        if (bits & vk::SurfaceTransformFlagBitsKHR::eRotate90) result.emplace_back("90 degree rotation");
        if (bits & vk::SurfaceTransformFlagBitsKHR::eRotate180) result.emplace_back("180 degree rotation");
        if (bits & vk::SurfaceTransformFlagBitsKHR::eRotate270) result.emplace_back("270 degree rotation");
        if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirror) result.emplace_back("horizantal mirror");
        if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate90) result.emplace_back("horizantal mirror, then 90 degree rotation");
        if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate180) result.emplace_back("horizantal mirror, then 180 degree rotation");
        if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate270)  result.emplace_back("horizantal mirror, then 270 degree rotation");
        if (bits & vk::SurfaceTransformFlagBitsKHR::eInherit) result.emplace_back("inherited");

        return result;
    }

    std::vector<std::string> LogAlphaCompositeBits(vk::CompositeAlphaFlagsKHR bits) {
        std::vector<std::string> result;
        if (bits & vk::CompositeAlphaFlagBitsKHR::eOpaque) result.emplace_back("opaque (alpha ignored)");
        if (bits & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied) result.emplace_back("pre multiplied (alpha expected to already be multiplied in image)");
        if (bits & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied) result.emplace_back("post multiplied (alpha will be applied during composition)");
        if (bits &vk::CompositeAlphaFlagBitsKHR::eInherit) result.emplace_back("inherited");

        return result;
    }

    std::vector<std::string> LogImageUsageBits(vk::ImageUsageFlags bits){
        std::vector<std::string> result;
        if (bits & vk::ImageUsageFlagBits::eTransferSrc) {
            result.emplace_back("transfer src: image can be used as the source of a transfer command.");
        }
        if (bits & vk::ImageUsageFlagBits::eTransferDst) {
            result.emplace_back("transfer dst: image can be used as the destination of a transfer command.");
        }
        if (bits & vk::ImageUsageFlagBits::eSampled) {
            result.emplace_back("sampled: image can be used to create a VkImageView suitable for occupying a \
VkDescriptorSet slot either of type VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE or \
VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, and be sampled by a shader.");
        }
        if (bits & vk::ImageUsageFlagBits::eStorage) {
            result.emplace_back("storage: image can be used to create a VkImageView suitable for occupying a \
VkDescriptorSet slot of type VK_DESCRIPTOR_TYPE_STORAGE_IMAGE.");
        }
        if (bits & vk::ImageUsageFlagBits::eColorAttachment) {
            result.emplace_back("color attachment: image can be used to create a VkImageView suitable for use as \
a color or resolve attachment in a VkFramebuffer.");
        }
        if (bits & vk::ImageUsageFlagBits::eDepthStencilAttachment) {
            result.emplace_back("depth/stencil attachment: image can be used to create a VkImageView \
suitable for use as a depth/stencil or depth/stencil resolve attachment in a VkFramebuffer.");
        }
        if (bits & vk::ImageUsageFlagBits::eTransientAttachment) {
            result.emplace_back("transient attachment: implementations may support using memory allocations \
with the VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT to back an image with this usage. This \
bit can be set for any image that can be used to create a VkImageView suitable for use as \
a color, resolve, depth/stencil, or input attachment.");
        }
        if (bits & vk::ImageUsageFlagBits::eInputAttachment) {
            result.emplace_back("input attachment: image can be used to create a VkImageView suitable for \
occupying VkDescriptorSet slot of type VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT; be read from \
a shader as an input attachment; and be used as an input attachment in a framebuffer.");
        }
        if (bits & vk::ImageUsageFlagBits::eFragmentDensityMapEXT) {
            result.emplace_back("fragment density map: image can be used to create a VkImageView suitable \
for use as a fragment density map image.");
        }
        if (bits & vk::ImageUsageFlagBits::eFragmentShadingRateAttachmentKHR) {
            result.emplace_back("fragment shading rate attachment: image can be used to create a VkImageView \
suitable for use as a fragment shading rate attachment or shading rate image");
        }
        return result;
    }

    std::string LogPresentMode(vk::PresentModeKHR presentMode){
        if(presentMode == vk::PresentModeKHR::eImmediate) return "present mod: immediate";
        if (presentMode == vk::PresentModeKHR::eMailbox) return "present mod: mailbox";
        if (presentMode == vk::PresentModeKHR::eFifo) return "present mod: fifo";
        if (presentMode == vk::PresentModeKHR::eFifoRelaxed) return "present mod: fifo relaxed";
        return "present mod: unknown";
    }

    void LogDeviceProperties(const vk::PhysicalDevice& device){
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

};