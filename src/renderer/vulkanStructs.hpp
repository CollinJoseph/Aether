#pragma once
#include "vmaUsage.hpp"
#include <glm/glm.hpp>

namespace Renderer {
struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  [[nodiscard]] bool isComplete() const {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

struct SwapChainInfo {
  VkFormat format;
  VkExtent2D extent;
};

struct UploadContext {
  VkFence fence;
  VkCommandBuffer commandBuffer;
};

struct CopyBufferInfo {
  VkBuffer srcBuffer;
  VkBuffer dstBuffer;
  VkDeviceSize size;
};

struct UniformBufferObject {
  glm::mat4 projection;
  glm::mat4 view;
};

struct AllocatedBuffer {
  VkBuffer buffer;
  VmaAllocation allocation;
  VmaAllocationInfo info;
};
} // namespace Renderer