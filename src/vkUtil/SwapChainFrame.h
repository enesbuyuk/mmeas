#pragma once
#include "../config.h"

namespace vkUtil {
    struct SwapChainFrame {
        vk::Image image;
        vk::ImageView imageView;
    };
}