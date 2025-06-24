#include "vulkanContext.hpp"
#include "rendererCommon.hpp"

namespace Renderer {

VulkanContext::VulkanContext(GLFWwindow *window) {
  m_window = window;
  createInstance();
  createSurface();
  pickPhysicalDevice();
  createLogicalDevice();
  createSwapChain();
  createImageViews();
  createCommandPool();
  createCommandBuffers();
  createSyncObjects();
}

void VulkanContext::createInstance() {
  // Vulkan Application info
  VkApplicationInfo appInfo = {};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  // TODO: Get the following info from the user
  appInfo.pApplicationName = "Playground";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);

  appInfo.pEngineName = "Aether";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

  appInfo.apiVersion = VK_API_VERSION_1_0;

  // Vulkan instance info to create the vulkan instance
  VkInstanceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  // Enable the extensions required by glfw
  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions =
      glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  createInfo.enabledExtensionCount = glfwExtensionCount;
  createInfo.ppEnabledExtensionNames = glfwExtensions;

  // Used to enable layers
  createInfo.enabledLayerCount = 0;

  if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS) {
    throw std::runtime_error("failed to create instance!");
  }
}

void VulkanContext::pickPhysicalDevice() {
  uint32_t physicalDeviceCount = 0;
  vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, nullptr);
  if (physicalDeviceCount == 0) {
    throw std::runtime_error("failed to enumerate physical devices!");
  }

  std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
  vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount,
                             physicalDevices.data());

  for (const auto &device : physicalDevices) {
    if (isDeviceSuitable(device)) {
      m_physicalDevice = device;
      break;
    }
  }

  if (m_physicalDevice == VK_NULL_HANDLE) {
    throw std::runtime_error("failed to create physical device!");
  }
}

bool VulkanContext::isDeviceSuitable(VkPhysicalDevice device) const {
  QueueFamilyIndices indices = getQueueFamilies(device);
  const bool extensionSupported = checkDeviceExtensionSupport(device);

  bool swapChainAdequate = false;
  if (extensionSupported) {
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
    swapChainAdequate = !swapChainSupport.formats.empty() &&
                        !swapChainSupport.presentModes.empty();
  }

  return indices.isComplete() && extensionSupported && swapChainAdequate;
}

bool VulkanContext::checkDeviceExtensionSupport(VkPhysicalDevice device) const {
  uint32_t extensionCount = 0;
  vkEnumerateDeviceExtensionProperties(device, VK_NULL_HANDLE, &extensionCount,
                                       nullptr);

  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(device, VK_NULL_HANDLE, &extensionCount,
                                       availableExtensions.data());

  std::set<std::string> requiredExtensions(deviceExtensions.begin(),
                                           deviceExtensions.end());

  for (const auto &extension : availableExtensions) {
    requiredExtensions.erase(extension.extensionName);
  }
  return requiredExtensions.empty();
}

QueueFamilyIndices
VulkanContext::getQueueFamilies(VkPhysicalDevice device) const {
  QueueFamilyIndices indices;
  uint32_t queueFamilyCount = 0;

  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                           queueFamilies.data());

  int i = 0;
  for (const auto &queue : queueFamilies) {
    if (queue.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
    }
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);
    if (presentSupport) {
      indices.presentFamily = i;
    }

    if (indices.isComplete()) {
      break;
    }
    i++;
  }

  return indices;
}

void VulkanContext::createLogicalDevice() {
  QueueFamilyIndices indices = getQueueFamilies(m_physicalDevice);
  float queuePriority = 1.0f;
  std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(),
                                            indices.presentFamily.value()};

  VkDeviceQueueCreateInfo queueCreateInfo = {};
  queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo.queueCount = 1;
  queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
  queueCreateInfo.pQueuePriorities = &queuePriority;

  VkPhysicalDeviceFeatures deviceFeatures = {};

  VkDeviceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.queueCreateInfoCount = 1;
  createInfo.pQueueCreateInfos = &queueCreateInfo;
  createInfo.pEnabledFeatures = &deviceFeatures;

  // INFO: Device extension goes here !!!
  createInfo.enabledExtensionCount =
      static_cast<uint32_t>(deviceExtensions.size());
  createInfo.ppEnabledExtensionNames = deviceExtensions.data();

  if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create logical device!");
  }

  vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0,
                   &m_graphicsQueue);
  vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);
}

void VulkanContext::createSwapChain() {
  SwapChainSupportDetails swapChainSupport =
      querySwapChainSupport(m_physicalDevice);

  VkSurfaceFormatKHR surfaceFormat =
      chooseSwapSurfaceFormat(swapChainSupport.formats);
  VkPresentModeKHR presentMode =
      chooseSwapPresentMode(swapChainSupport.presentModes);
  VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

  uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
  if (swapChainSupport.capabilities.maxImageCount > 0 &&
      imageCount > swapChainSupport.capabilities.maxImageCount) {
    imageCount = swapChainSupport.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = m_surface;

  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  QueueFamilyIndices indices = getQueueFamilies(m_physicalDevice);
  uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
                                   indices.presentFamily.value()};

  if (indices.graphicsFamily != indices.presentFamily) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;

  createInfo.oldSwapchain = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapchain) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create swap chain!");
  }

  vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, nullptr);
  m_swapchainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount,
                          m_swapchainImages.data());

  m_swapchainImageFormat = surfaceFormat.format;
  m_swapchainExtent = extent;
}

VkSurfaceFormatKHR VulkanContext::chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR> &availableFormats) {
  for (const auto &availableFormat : availableFormats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
        availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return availableFormat;
    }
  }

  return availableFormats[0];
}

VkPresentModeKHR VulkanContext::chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR> &availablePresentModes) {
  for (const auto &availablePresentMode : availablePresentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return availablePresentMode;
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D
VulkanContext::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
  if (capabilities.currentExtent.width !=
      std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  } else {
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);

    VkExtent2D actualExtent = {static_cast<uint32_t>(width),
                               static_cast<uint32_t>(height)};

    actualExtent.width =
        std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                   capabilities.maxImageExtent.width);
    actualExtent.height =
        std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                   capabilities.maxImageExtent.height);

    return actualExtent;
  }
}

SwapChainSupportDetails
VulkanContext::querySwapChainSupport(VkPhysicalDevice device) const {
  SwapChainSupportDetails details;

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface,
                                            &details.capabilities);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount,
                                       nullptr);

  if (formatCount != 0) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount,
                                         details.formats.data());
  }

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface,
                                            &presentModeCount, nullptr);

  if (presentModeCount != 0) {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, m_surface, &presentModeCount, details.presentModes.data());
  }

  return details;
}

void VulkanContext::createImageViews() {
  m_swapchainImageViews.resize(m_swapchainImages.size());

  for (size_t i = 0; i < m_swapchainImages.size(); i++) {
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = m_swapchainImages[i];
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = m_swapchainImageFormat;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(m_device, &createInfo, nullptr,
                          &m_swapchainImageViews[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create image views!");
    }
  }
}

void VulkanContext::createSurface() {
  if (glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create window surface!");
  }
}

void VulkanContext::createCommandPool() {
  const QueueFamilyIndices queueFamilyIndices =
      getQueueFamilies(m_physicalDevice);

  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

  if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool)) {
    throw std::runtime_error("failed to create command pool!");
  }
}

void VulkanContext::createCommandBuffers() {
  m_commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = m_commandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

  if (vkAllocateCommandBuffers(m_device, &allocInfo, m_commandBuffers.data()) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }
}

void VulkanContext::createSyncObjects() {
  m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  m_renderFinishedSemaphores.resize(m_swapchainImages.size());
  m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr,
                          &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
        vkCreateFence(m_device, &fenceInfo, nullptr, &m_inFlightFences[i]) !=
            VK_SUCCESS) {

      throw std::runtime_error(
          "failed to create synchronization objects for a frame!");
    }
  }

  for (size_t i = 0; i < m_swapchainImages.size(); i++) {
    if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr,
                          &m_renderFinishedSemaphores[i]) != VK_SUCCESS) {
      throw std::runtime_error(
          "failed to create synchronization objects for a frame!");
    }
  }
}

void VulkanContext::destroySyncObjects() {
  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(m_device, m_imageAvailableSemaphores[i], nullptr);
    vkDestroyFence(m_device, m_inFlightFences[i], nullptr);
  }

  for (size_t i = 0; i < m_swapchainImages.size(); i++) {
    vkDestroySemaphore(m_device, m_renderFinishedSemaphores[i], nullptr);
  }
}

void VulkanContext::destroySwapChain() {
  for (auto imageView : m_swapchainImageViews) {
    vkDestroyImageView(m_device, imageView, nullptr);
  }
  vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
}

VkDevice VulkanContext::getDevice() const { return m_device; }

SwapChainInfo VulkanContext::getSwapChainInfo() const {
  return {m_swapchainImageFormat, m_swapchainExtent};
}

const std::vector<VkImageView> &VulkanContext::getImageViews() const {
  return m_swapchainImageViews;
}

const std::vector<VkCommandBuffer> &VulkanContext::getCommandBuffers() const {
  return m_commandBuffers;
}

void VulkanContext::beginFrame() {
  vkWaitForFences(m_device, 1, &m_inFlightFences[m_currentFrame], VK_TRUE,
                  UINT64_MAX);
}

void VulkanContext::resetCurrentInFlightFence() {
  vkResetFences(m_device, 1, &m_inFlightFences[m_currentFrame]);
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
}

int64_t VulkanContext::acquireNextSwapChainImage() {

  /*
   * INFO: Making a guess that image index is always going to positive, to
   *  return the swapchain is suboptimal using -1
   */

  uint32_t imageIndex;
  VkResult result = vkAcquireNextImageKHR(
      m_device, m_swapchain, UINT64_MAX,
      m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &imageIndex);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
    return -1;
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to present swap chain image!");
  }
  return imageIndex;
}

void VulkanContext::presentFrame(uint32_t imageIndex) {

  VkSemaphore waitSemaphores[] = {m_imageAvailableSemaphores[m_currentFrame]};
  VkSemaphore signalSemaphores[] = {m_renderFinishedSemaphores[imageIndex]};

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = {m_swapchain};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;

  presentInfo.pImageIndices = &imageIndex;

  vkQueuePresentKHR(m_presentQueue, &presentInfo);
}

void VulkanContext::nextFrame() {
  m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VulkanContext::recreateSwapChain() {
  destroySwapChain();
  createSwapChain();
  createImageViews();
}

uint32_t VulkanContext::getCurrentFrame() const { return m_currentFrame; }

void VulkanContext::waitForDeviceIdle() const { vkDeviceWaitIdle(m_device); }

VulkanContext::~VulkanContext() {
  destroySwapChain();
  destroySyncObjects();

  vkDestroyCommandPool(m_device, m_commandPool, nullptr);

  vkDestroyDevice(m_device, nullptr);
  vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
  vkDestroyInstance(m_instance, nullptr);
}

} // namespace Renderer