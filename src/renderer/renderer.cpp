#include "renderer.hpp"

#include "vulkan/vulkanBuffer.hpp"
#include "vulkan/vulkanHelpers.hpp"

#include <glm/gtx/transform.hpp>

namespace Aether::Renderer {
Renderer::Renderer(Window &window) : m_context(window) {
  createDescriptorSetLayout();
  createVertexBuffer();
  // createUniformBuffers();
  createGraphicsPipeline();
}

void Renderer::createDescriptorSetLayout() {
  VkDescriptorSetLayoutBinding bindings{};
  bindings.binding = 0;
  bindings.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  bindings.descriptorCount = 1;
  bindings.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  bindings.pImmutableSamplers = nullptr;

  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = 1;
  layoutInfo.pBindings = &bindings;
  if (vkCreateDescriptorSetLayout(m_context.getDevice(), &layoutInfo, nullptr,
                                  &m_descriptorSetLayout)) {
    throw std::runtime_error("failed to create descriptor set layout!");
  };
}

void Renderer::createVertexBuffer() {
  const size_t bufferSize = vertices.size() * sizeof(Vertex);

  Vulkan::VulkanBuffer stagingBuffer(
      m_context.getAllocator(), bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VMA_MEMORY_USAGE_AUTO,
      VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
  stagingBuffer.copyDataToBuffer(vertices.data(), bufferSize);

  m_vertexBuffer = std::make_unique<Vulkan::VulkanBuffer>(
      m_context.getAllocator(), bufferSize,
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);

  VkCommandBuffer commandBuffer =
      Vulkan::VulkanHelpers::beginSingleTimeCommands(m_context);
  VkBufferCopy copyRegion{0, 0, bufferSize};
  vkCmdCopyBuffer(commandBuffer, stagingBuffer.getBuffer(),
                  m_vertexBuffer->getBuffer(), 1, &copyRegion);
  Vulkan::VulkanHelpers::endSingleTimeCommands(m_context, commandBuffer);
}

void Renderer::createUniformBuffers() {
  VkDeviceSize bufferSize = sizeof(Vulkan::UniformBufferObject);
  for (int i = 0; i < Vulkan::MAX_FRAMES_IN_FLIGHT; i++) {
    m_uniformBuffers.emplace_back(m_context.getAllocator(), bufferSize,
                                  VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                  VMA_MEMORY_USAGE_AUTO);
  }
}

void Renderer::updateUniformBuffers(uint32_t currentImage) {
  Vulkan::UniformBufferObject ubo{};
  Vulkan::SwapChainInfo swapChainInfo = m_context.getSwapChainInfo();
  ubo.view =
      glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, 0.0f, 1.0f));
  ubo.projection = glm::perspective(glm::radians(45.0f),
                                    (float)swapChainInfo.extent.width /
                                        (float)swapChainInfo.extent.height,
                                    0.1f, 10.0f);

  m_uniformBuffers[currentImage].copyDataToBuffer(
      &ubo, sizeof(Vulkan::UniformBufferObject));
}

void Renderer::drawFrame() {
  const auto imageIntex = m_context.beginFrame();
  const auto currentFrame = m_context.getCurrentFrame();
  if (imageIntex < 0) {
    return;
  }
  // updateUniformBuffers(currentFrame);
  vkResetCommandBuffer(m_context.getCommandBuffers()[currentFrame], 0);
  recordCommandBuffer(m_context.getCommandBuffers()[currentFrame], imageIntex);
  m_context.endFrame(imageIntex);
}

std::vector<char> Renderer::readFile(const std::string &filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("failed to open file!");
  }

  size_t fileSize = (size_t)file.tellg();
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);

  file.close();

  return buffer;
}

void Renderer::createGraphicsPipeline() {
  // Shader stages

  auto vertexShader = readFile("../res/shaders/shader.vert.spv");
  auto fragmentShader = readFile("../res/shaders/shader.frag.spv");

  VkShaderModule vertShaderModule = createShaderModule(vertexShader);
  VkShaderModule fragShaderModule = createShaderModule(fragmentShader);

  VkPipelineShaderStageCreateInfo vertShaderStageCreateInfo = {};
  vertShaderStageCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageCreateInfo.module = vertShaderModule;
  vertShaderStageCreateInfo.pName = "main";

  VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo = {};
  fragShaderStageCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageCreateInfo.module = fragShaderModule;
  fragShaderStageCreateInfo.pName = "main";

  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageCreateInfo,
                                                    fragShaderStageCreateInfo};

  // Fixed Funtion stages
  auto bindingDesc = Vertex::getBindingDescription();
  auto attributeDescs = Vertex::getAttributeDescriptions();

  VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
  vertexInputStateCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
  vertexInputStateCreateInfo.vertexAttributeDescriptionCount =
      attributeDescs.size();
  vertexInputStateCreateInfo.pVertexAttributeDescriptions =
      attributeDescs.data();
  vertexInputStateCreateInfo.pVertexBindingDescriptions = &bindingDesc;

  VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
  inputAssembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;

  VkPipelineMultisampleStateCreateInfo multisampling{};
  multisampling.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  // TODO: Go back and read
  // https://docs.vulkan.org/tutorial/latest/03_Drawing_a_triangle/02_Graphics_pipeline_basics/02_Fixed_functions.html#_color_blending

  VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  colorBlendAttachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_FALSE;

  VkPipelineColorBlendStateCreateInfo colorBlending{};
  colorBlending.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY;
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachment;
  colorBlending.blendConstants[0] = 0.0f;
  colorBlending.blendConstants[1] = 0.0f;
  colorBlending.blendConstants[2] = 0.0f;
  colorBlending.blendConstants[3] = 0.0f;

  // Dynamic stages

  std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT,
                                               VK_DYNAMIC_STATE_SCISSOR};

  VkPipelineDynamicStateCreateInfo dynamicState{};
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
  dynamicState.pDynamicStates = dynamicStates.data();

  VkPipelineViewportStateCreateInfo viewportState{};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.scissorCount = 1;

  // INFO: Create pipeline layout

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 1;
  pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;

  if (vkCreatePipelineLayout(m_context.getDevice(), &pipelineLayoutInfo,
                             nullptr, &m_pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;
  pipelineInfo.pVertexInputState = &vertexInputStateCreateInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &dynamicState;

  pipelineInfo.layout = m_pipelineLayout;
  pipelineInfo.renderPass = m_context.getRenderPass();
  pipelineInfo.subpass = 0;

  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
  pipelineInfo.basePipelineIndex = -1;

  auto d = m_context.getDevice();
  if (vkCreateGraphicsPipelines(d, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr,
                                &m_graphicsPipeline) != VK_SUCCESS) {
    throw std::runtime_error("failed to create graphics pipeline!");
  }

  vkDestroyShaderModule(m_context.getDevice(), vertShaderModule, nullptr);
  vkDestroyShaderModule(m_context.getDevice(), fragShaderModule, nullptr);
}

void Renderer::recordCommandBuffer(VkCommandBuffer commandBuffer,
                                   uint32_t imageIndex) {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer!");
  }

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = m_context.getRenderPass();
  renderPassInfo.framebuffer = m_context.getFramebuffers()[imageIndex];
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = m_context.getSwapChainInfo().extent;

  VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
  renderPassInfo.clearValueCount = 1;
  renderPassInfo.pClearValues = &clearColor;

  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    m_graphicsPipeline);

  VkBuffer vertexBuffers[] = {m_vertexBuffer->getBuffer()};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

  auto swapChainExtent = m_context.getSwapChainInfo().extent;

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(swapChainExtent.width);
  viewport.height = static_cast<float>(swapChainExtent.height);
  viewport.minDepth = 0.0;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = swapChainExtent;
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

  vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);

  vkCmdEndRenderPass(commandBuffer);

  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }
}

VkShaderModule Renderer::createShaderModule(std::vector<char> &shaderCode) {
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = shaderCode.size();
  createInfo.pCode = reinterpret_cast<const uint32_t *>(shaderCode.data());
  VkShaderModule shaderModule = VK_NULL_HANDLE;
  if (vkCreateShaderModule(m_context.getDevice(), &createInfo, nullptr,
                           &shaderModule) != VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module!");
  }
  return shaderModule;
}

Renderer::~Renderer() {
  m_context.waitForIdle();
  // for (AllocatedBuffer buffer : uniformBuffers) {
  //   m_context.destroyBuffer(buffer);
  // }
  vkDestroyDescriptorSetLayout(m_context.getDevice(), m_descriptorSetLayout,
                               nullptr);
  vkDestroyPipeline(m_context.getDevice(), m_graphicsPipeline, nullptr);
  vkDestroyPipelineLayout(m_context.getDevice(), m_pipelineLayout, nullptr);
}

} // namespace Aether::Renderer