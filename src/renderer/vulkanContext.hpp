#pragma once
#include "pch/pch.hpp"
#include "vulkanStructs.hpp"

namespace Renderer {
class VulkanContext {
public:
  explicit VulkanContext(GLFWwindow *window);
  ~VulkanContext();
  VkDevice getDevice() const;
  SwapChainInfo getSwapChainInfo() const;
  const std::vector<VkImageView> &getImageViews() const;
  const std::vector<VkCommandBuffer> &getCommandBuffers() const;
  void beginFrame();
  void endFrame(uint32_t imageIndex);
  void waitIdle() const;
  // TODO: Move to another class
  uint32_t acquireNextSwapChainImage();
  void presentFrame(uint32_t imageIndex);
  void nextFrame();
  uint32_t getCurrentFrame() const;

private:
  inline static const std::vector<const char *> deviceExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  GLFWwindow *m_window = nullptr;
  VkInstance m_instance = VK_NULL_HANDLE;
  VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
  VkDevice m_device = VK_NULL_HANDLE;
  VkQueue m_graphicsQueue = VK_NULL_HANDLE;
  VkQueue m_presentQueue = VK_NULL_HANDLE;
  VkSurfaceKHR m_surface = VK_NULL_HANDLE;
  VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
  VkFormat m_swapchainImageFormat = VK_FORMAT_UNDEFINED;
  VkExtent2D m_swapchainExtent = {};
  VkCommandPool m_commandPool = VK_NULL_HANDLE;
  std::vector<VkImage> m_swapchainImages = {};
  std::vector<VkImageView> m_swapchainImageViews = {};
  std::vector<VkCommandBuffer> m_commandBuffers;
  std::vector<VkSemaphore> m_imageAvailableSemaphores;
  std::vector<VkSemaphore> m_renderFinishedSemaphores;
  std::vector<VkFence> m_inFlightFences;
  uint32_t m_currentFrame = 0;

private:
  void createInstance();
  void pickPhysicalDevice();
  bool isDeviceSuitable(VkPhysicalDevice device) const;
  bool checkDeviceExtensionSupport(VkPhysicalDevice device) const;
  QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device) const;
  void createLogicalDevice();
  void createSwapChain();
  VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);
  VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
  SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) const;
  void createImageViews();
  void createSurface();
  void createCommandPool();
  void createCommandBuffers();
  void createSyncObjects();
};
} // namespace Renderer