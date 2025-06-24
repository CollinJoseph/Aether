#pragma once
#include "engine/window.hpp"
#include "vulkanContext.hpp"

namespace Renderer {
class Renderer {
public:
  explicit Renderer(Engine::Window &window);
  ~Renderer();
  void drawFrame();

private:
  Engine::Window &m_window;
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
  void destroyFramebuffers();
  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
  void recreateSwapChain();
};
} // namespace Renderer