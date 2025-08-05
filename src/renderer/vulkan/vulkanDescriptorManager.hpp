#pragma once
#include "renderer/vulkanContext.hpp"

namespace Aether::Renderer::Vulkan {

class VulkanDescriptorManager {
public:
  explicit VulkanDescriptorManager(::Renderer::VulkanContext &m_vulkan_context)
      : m_vulkanContext(m_vulkan_context) {}

private:
  ::Renderer::VulkanContext &m_vulkanContext;
};

} // namespace Aether::Renderer::Vulkan