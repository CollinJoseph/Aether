#pragma once
#include "pch/pch.hpp"
#include "renderer/vulkanContext.hpp"
#include "vulkanBuffer.hpp"

namespace Aether::Renderer::Vulkan {
class VulkanHelpers {
public:
  static VkCommandBuffer beginSingleTimeCommands(const VulkanContext &context);
  static void endSingleTimeCommands(VulkanContext &context,
                                    VkCommandBuffer commandBuffer);
};
} // namespace Aether::Renderer::Vulkan