#include "vulkanDescriptorManager.hpp"

namespace Aether::Renderer::Vulkan {
VulkanDescriptorManager::VulkanDescriptorManager(VulkanContext &context)
    : m_context(context) {
  createDescriptorSetLayout();
  createDescriptorPool();
  createDescriptorSets();
}

VulkanDescriptorManager::~VulkanDescriptorManager() {
  vkDestroyDescriptorPool(m_context.getDevice(), m_descriptorPool, nullptr);
  vkDestroyDescriptorSetLayout(m_context.getDevice(), m_descriptorSetLayout,
                               nullptr);
}

const VkDescriptorSetLayout *
VulkanDescriptorManager::getDescriptorSetLayout() const {
  return &m_descriptorSetLayout;
}

const std::vector<VkDescriptorSet> &
VulkanDescriptorManager::getDescriptorSets() const {
  return m_descriptorSets;
}

void VulkanDescriptorManager::createDescriptorSetLayout() {
  VkDescriptorSetLayoutBinding uboLayoutBinding{
      .binding = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .descriptorCount = 1,
      .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
      .pImmutableSamplers = nullptr,
  };

  VkDescriptorSetLayoutCreateInfo uboLayoutCreateInfo{
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .bindingCount = 1,
      .pBindings = &uboLayoutBinding,
  };

  if (vkCreateDescriptorSetLayout(m_context.getDevice(), &uboLayoutCreateInfo,
                                  nullptr,
                                  &m_descriptorSetLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
  }
}

void VulkanDescriptorManager::createDescriptorPool() {
  VkDescriptorPoolSize poolSize = {
      .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT)};
  VkDescriptorPoolCreateInfo poolInfo = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
      .maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT),
      .poolSizeCount = 1,
      .pPoolSizes = &poolSize,
  };
  if (vkCreateDescriptorPool(m_context.getDevice(), &poolInfo, nullptr,
                             &m_descriptorPool) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor pool!");
  }
}

void VulkanDescriptorManager::createDescriptorSets() {
  std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT,
                                             m_descriptorSetLayout);
  VkDescriptorSetAllocateInfo allocInfo{
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
      .descriptorPool = m_descriptorPool,
      .descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT),
      .pSetLayouts = layouts.data(),
  };
  m_descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
  if (vkAllocateDescriptorSets(m_context.getDevice(), &allocInfo,
                               m_descriptorSets.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate descriptor sets!");
  }
}
} // namespace Aether::Renderer::Vulkan