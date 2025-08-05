#pragma once
#include "pch/pch.hpp"
#include "vulkan/vulkanBuffer.hpp"
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
  void drawFrame();
  const std::vector<Vertex> vertices = {
      {{0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}}, // Bottom-left
      {{1.5f, 0.5f}, {0.0f, 1.0f, 0.0f}}, // Bottom-right
      {{1.5f, 1.5f}, {0.0f, 0.0f, 1.0f}}, // Top-right

      {{1.5f, 1.5f}, {0.0f, 0.0f, 1.0f}}, // Top-right
      {{0.5f, 1.5f}, {1.0f, 1.0f, 0.0f}}, // Top-left
      {{0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}}, // Bottom-left
  };

private:
  Vulkan::VulkanContext m_context;
  VkDescriptorSetLayout m_descriptorSetLayout;
  VkPipelineLayout m_pipelineLayout;
  VkPipeline m_graphicsPipeline;

  const std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};
  std::unique_ptr<Vulkan::VulkanBuffer> m_vertexBuffer;
  std::vector<Vulkan::VulkanBuffer> m_uniformBuffers;

private:
  static std::vector<char> readFile(const std::string &filename);
  VkShaderModule createShaderModule(std::vector<char> &shaderCode);
  void createGraphicsPipeline();
  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
  void createDescriptorSetLayout();
  void createVertexBuffer();
  void createUniformBuffers();
  void updateUniformBuffers(uint32_t currentImage);
  void destroyUniformBuffers();
};
} // namespace Aether::Renderer