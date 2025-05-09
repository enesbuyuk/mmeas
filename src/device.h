#pragma once
#include "config.h"

namespace vkInit {
    vk::PhysicalDevice choose_physical_device(vk::Instance& instance, bool debug){
        if (debug) std::cout<< "choosing physical device" << "\n";
        return nullptr;
    }
}