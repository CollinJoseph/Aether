#include "vulkanDescriptorManager.hpp"

#include "config/config.hpp"

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
  VkDescriptorSetLayoutBinding viewProjectMatrixLayoutBinding{
      .binding = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .descriptorCount = 1,
      .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
      .pImmutableSamplers = nullptr,
  };

  VkDescriptorSetLayoutBinding vertexSSBOLayoutBinding{
      .binding = 1,
      .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
      .descriptorCount = 1,
      .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
      .pImmutableSamplers = nullptr};

  VkDescriptorSetLayoutBinding layoutBinding[2] = {
      viewProjectMatrixLayoutBinding, vertexSSBOLayoutBinding};

  VkDescriptorSetLayoutCreateInfo uboLayoutCreateInfo{
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .bindingCount = 2,
      .pBindings = layoutBinding,
  };

  if (vkCreateDescriptorSetLayout(m_context.getDevice(), &uboLayoutCreateInfo,
                                  nullptr,
                                  &m_descriptorSetLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
  }
}

void VulkanDescriptorManager::createDescriptorPool() {
  VkDescriptorPoolSize poolSize[] = {
      {.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
       .descriptorCount = static_cast<uint32_t>(Config::MAX_FRAMES_IN_FLIGHT)},
      {.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
       .descriptorCount = static_cast<uint32_t>(Config::MAX_FRAMES_IN_FLIGHT)},
  };
  VkDescriptorPoolCreateInfo poolInfo = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
      .maxSets = static_cast<uint32_t>(Config::MAX_FRAMES_IN_FLIGHT),
      .poolSizeCount = 2,
      .pPoolSizes = poolSize,
  };
  if (vkCreateDescriptorPool(m_context.getDevice(), &poolInfo, nullptr,
                             &m_descriptorPool) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor pool!");
  }
}

void VulkanDescriptorManager::createDescriptorSets() {
  std::vector<VkDescriptorSetLayout> layouts(Config::MAX_FRAMES_IN_FLIGHT,
                                             m_descriptorSetLayout);
  VkDescriptorSetAllocateInfo allocInfo{
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
      .descriptorPool = m_descriptorPool,
      .descriptorSetCount = static_cast<uint32_t>(Config::MAX_FRAMES_IN_FLIGHT),
      .pSetLayouts = layouts.data(),
  };
  m_descriptorSets.resize(Config::MAX_FRAMES_IN_FLIGHT);
  if (vkAllocateDescriptorSets(m_context.getDevice(), &allocInfo,
                               m_descriptorSets.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate descriptor sets!");
  }
}
} // namespace Aether::Renderer::Vulkan