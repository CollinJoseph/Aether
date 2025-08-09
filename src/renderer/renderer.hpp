#pragma once
#include "config/config.hpp"
#include "pch/pch.hpp"
#include "vulkan/vulkanBuffer.hpp"
#include "vulkan/vulkanDescriptorManager.hpp"
#include "vulkanContext.hpp"
#include "window.hpp"

#include <glm/glm.hpp>
#include <memory>

namespace Aether::Renderer {

struct Vertex {
  glm::vec2 position;
  glm::vec3 color;

  static VkVertexInputBindingDescription getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescription;
  }

  static std::array<VkVertexInputAttributeDescription, 2>
  getAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].offset = offsetof(Vertex, color);
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    return attributeDescriptions;
  }
};

class Renderer {
public:
  explicit Renderer(Window &window);
  ~Renderer();
  void write2dTransformsToBuffer(const std::vector<glm::mat4> &transforms);
  void render();

private:
  Vulkan::VulkanContext m_context;
  Vulkan::VulkanDescriptorManager m_descriptorManager;
  VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
  VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;

  std::unique_ptr<Vulkan::VulkanBuffer> m_vertexBuffer;
  std::vector<Vulkan::VulkanBuffer> m_uniformBuffers;

  uint32_t m_renderObjectsCount = 0;
  std::unique_ptr<Vulkan::VulkanBuffer> m_transform2dStagingBuffer;
  bool m_transform2dChanged[Config::MAX_FRAMES_IN_FLIGHT] = {true};
  std::vector<Vulkan::VulkanBuffer> m_transform2dSSBO;

  const std::vector<Vertex> vertices = {
      {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}}, {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
      {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},

      {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},   {{-0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}},
      {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
  };

private:
  static std::vector<char> readFile(const std::string &filename);

  VkShaderModule createShaderModule(std::vector<char> &shaderCode);
  void createGraphicsPipeline();
  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

  void createBuffers();
  void writeVertexDataToVertexBuffer();
  void bindBuffersToDescriptorSets() const;
  void updateUniformBuffers(uint32_t currentFrame) const;
  void updateTransform2dBuffers(uint32_t currentFrame);
};
} // namespace Aether::Renderer