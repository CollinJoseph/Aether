#include "config/config.hpp"
#include "vulkanContext.hpp"

namespace Aether::Renderer::Vulkan {
int64_t VulkanContext::beginFrame() {
  vkWaitForFences(m_device, 1, &m_inFlightFences[m_currentFrame], VK_TRUE,
                  UINT64_MAX);
  uint32_t imageIndex;
  VkResult result = vkAcquireNextImageKHR(
      m_device, m_swapchain, UINT64_MAX,
      m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &imageIndex);
  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapChain();
    return -1;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to present swap chain image!");
  }
  vkResetFences(m_device, 1, &m_inFlightFences[m_currentFrame]);
  return imageIndex;
}

void VulkanContext::endFrame(uint32_t imageIndex) {
  VkSemaphore waitSemaphores[] = {m_imageAvailableSemaphores[m_currentFrame]};
  VkSemaphore signalSemaphores[] = {m_renderFinishedSemaphores[imageIndex]};

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &m_commandBuffers[m_currentFrame];

  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo,
                    m_inFlightFences[m_currentFrame]) != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = {m_swapchain};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;

  presentInfo.pImageIndices = &imageIndex;

  if (const VkResult result = vkQueuePresentKHR(m_presentQueue, &presentInfo);
      result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      m_window.m_windowResized) {
    m_window.m_windowResized = false;
    recreateSwapChain();
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to present swap chain image!");
  }

  m_currentFrame = (m_currentFrame + 1) % Config::MAX_FRAMES_IN_FLIGHT;
}

uint32_t VulkanContext::getCurrentFrame() const { return m_currentFrame; }
} // namespace Aether::Renderer::Vulkan
