#pragma once
#include "renderer/vulkanContext.hpp"

namespace Aether::Renderer::Vulkan {

class VulkanDescriptorManager {
public:
  VulkanDescriptorManager(VulkanContext &context);
  ~VulkanDescriptorManager();
  const VkDescriptorSetLayout *getDescriptorSetLayout() const;
  const std::vector<VkDescriptorSet> &getDescriptorSets() const;

private:
  VulkanContext &m_context;
  VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
  VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
  std::vector<VkDescriptorSet> m_descriptorSets;

private:
  void createDescriptorSetLayout();
  void createDescriptorPool();
  void createDescriptorSets();
};

} // namespace Aether::Renderer::Vulkan