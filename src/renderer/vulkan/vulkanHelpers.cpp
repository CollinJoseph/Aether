#include "vulkanHelpers.hpp"

namespace Aether::Renderer::Vulkan {
VkCommandBuffer
VulkanHelpers::beginSingleTimeCommands(const VulkanContext &context) {
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = context.getCommandPool();
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(context.getDevice(), &allocInfo, &commandBuffer);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(commandBuffer, &beginInfo);
  return commandBuffer;
}

void VulkanHelpers::endSingleTimeCommands(VulkanContext &context,
                                          VkCommandBuffer commandBuffer) {

  vkEndCommandBuffer(commandBuffer);

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  VkFence fence;
  vkCreateFence(context.getDevice(), &fenceInfo, nullptr, &fence);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  vkQueueSubmit(context.getGraphicsQueue(), 1, &submitInfo, fence);
  vkWaitForFences(context.getDevice(), 1, &fence, VK_TRUE, UINT64_MAX);
  vkDestroyFence(context.getDevice(), fence, nullptr);
  vkFreeCommandBuffers(context.getDevice(), context.getCommandPool(), 1,
                       &commandBuffer);
}
} // namespace Aether::Renderer::Vulkan