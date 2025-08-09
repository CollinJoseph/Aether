#pragma once
#include "pch/pch.hpp"
#include "vmaUsage.hpp"
#include "vulkanStructs.hpp"
#include "window.hpp"

namespace Aether::Renderer::Vulkan {

class VulkanContext {
public:
  explicit VulkanContext(Window &window);
  VkDevice getDevice() const;
  void waitForIdle() const;
  const std::vector<VkCommandBuffer> &getCommandBuffers() const;
  [[nodiscard]] VkCommandPool getCommandPool() const;
  [[nodiscard]] VkQueue getGraphicsQueue() const;
  const std::vector<VkFramebuffer> &getFramebuffers() const;
  const VkRenderPass &getRenderPass() const;
  void destroyRenderPass();
  SwapChainInfo getSwapChainInfo() const;
  int64_t beginFrame();
  void endFrame(uint32_t imageIndex);
  uint32_t getCurrentFrame() const;
  VmaAllocator getAllocator() const;
  ~VulkanContext();

private:
  inline static const std::vector<const char *> deviceExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  Window &m_window;
  VkInstance m_instance = VK_NULL_HANDLE;
  VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
  VkDevice m_device = VK_NULL_HANDLE;
  VkQueue m_graphicsQueue = VK_NULL_HANDLE;
  VkQueue m_presentQueue = VK_NULL_HANDLE;
  VkSurfaceKHR m_surface = VK_NULL_HANDLE;
  VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
  VkFormat m_swapchainImageFormat = VK_FORMAT_UNDEFINED;
  VkExtent2D m_swapchainExtent = {};
  // Info: Command pools are externally synchronized, meaning that a command
  //  pool must not be used concurrently in multiple threads.
  VkCommandPool m_commandPool = VK_NULL_HANDLE;
  VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
  VkRenderPass m_renderPass;
  VmaAllocator m_allocator;
  std::vector<VkFramebuffer> m_swapChainFramebuffers;
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
  void createFramebuffers();
  void createSurface();
  void createCommandPool();
  void createCommandBuffers();
  void createSyncObjects();
  void destroySyncObjects();
  void createVMA();
  void destroySwapChain();
  void createRenderPass();
  void recreateSwapChain();
};
} // namespace Aether::Renderer::Vulkan