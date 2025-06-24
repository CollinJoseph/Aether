#pragma once
#include "vulkanContext.hpp"

namespace Renderer {
class Renderer {
public:
  Renderer(GLFWwindow *window);
  ~Renderer();
  void drawFrame();

private:
  VulkanContext m_context;
  VkRenderPass m_renderPass;
  VkPipelineLayout m_pipelineLayout;
  VkPipeline m_graphicsPipeline;
  std::vector<VkFramebuffer> m_swapChainFramebuffers;

private:
  static std::vector<char> readFile(const std::string &filename);
  VkShaderModule createShaderModule(std::vector<char> &shaderCode);
  void createRenderPass();
  void createGraphicsPipeline();
  void createFramebuffers();
  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
};
} // namespace Renderer