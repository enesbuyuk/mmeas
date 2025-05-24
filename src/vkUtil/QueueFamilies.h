#pragma once

#include "../config.h"

namespace vkUtil {

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool IsComplete() const { return graphicsFamily.has_value() && presentFamily.has_value(); }
    };

    QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface, bool debug) {
        QueueFamilyIndices indices;

        std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

        if (debug) std::cout << "system can support " << queueFamilies.size() << " queue families.\n";

        int i = 0;

        for (const auto &queueFamily: queueFamilies) {
            if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
                indices.graphicsFamily = i;
                indices.presentFamily = i;

                if (debug) std::cout << "queue family " << i << " is suitable for graphics and presenting." << "\n";
            }

            if (device.getSurfaceSupportKHR(i, surface)) {
                indices.presentFamily = i;
                if (debug) std::cout << "queue family " << i << " is suitable for presenting.\n";
            }

            if (indices.IsComplete()) break;
            i++;
        }

        return indices;
    }

}