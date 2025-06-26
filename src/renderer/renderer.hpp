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
  VulkanContext m_context;
  VkPipelineLayout m_pipelineLayout;
  VkPipeline m_graphicsPipeline;

private:
  static std::vector<char> readFile(const std::string &filename);
  VkShaderModule createShaderModule(std::vector<char> &shaderCode);
  void createGraphicsPipeline();
  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
};
} // namespace Renderer