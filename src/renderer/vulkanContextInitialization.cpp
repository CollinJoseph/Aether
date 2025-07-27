#include "vulkanContext.hpp"

namespace Renderer {

VulkanContext::VulkanContext(Engine::Window &window) : m_window(window) {
  createInstance();
  createSurface();
  pickPhysicalDevice();
  createLogicalDevice();
  createSwapChain();
  createVMA();
  createImageViews();
  createRenderPass();
  createFramebuffers();
  createCommandPool();
  createCommandBuffers();
  createSyncObjects();
  createUploadContext();
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

VkDevice VulkanContext::getDevice() const { return m_device; }

void VulkanContext::waitForIdle() const { vkDeviceWaitIdle(m_device); }

void VulkanContext::createSurface() {
  if (glfwCreateWindowSurface(m_instance, m_window.getWindow(), nullptr,
                              &m_surface) != VK_SUCCESS) {
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

const std::vector<VkCommandBuffer> &VulkanContext::getCommandBuffers() const {
  return m_commandBuffers;
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

void VulkanContext::createDescriptorPool() {
  VkDescriptorPoolSize poolSize{};
  poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
}

void VulkanContext::destroyDescriptorPool() {}

void VulkanContext::createVMA() {
  VmaAllocatorCreateInfo allocatorInfo{};
  allocatorInfo.flags = 0;
  allocatorInfo.device = m_device;
  allocatorInfo.physicalDevice = m_physicalDevice;
  allocatorInfo.instance = m_instance;

  if (vmaCreateAllocator(&allocatorInfo, &m_allocator) != VK_SUCCESS) {
    throw std::runtime_error("failed to create allocator!");
  }
}

VmaAllocator VulkanContext::getAllocator() const { return m_allocator; }

void VulkanContext::createUploadContext() {
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = m_commandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = 1;

  if (vkAllocateCommandBuffers(m_device, &allocInfo,
                               &m_uploadContext.commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate upload command buffer!");
  }

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

  if (vkCreateFence(m_device, &fenceInfo, nullptr, &m_uploadContext.fence)) {
    throw std::runtime_error("failed to create upload fence!");
  }
};

void VulkanContext::destroyUploadContext() {
  vkDestroyFence(m_device, m_uploadContext.fence, nullptr);
}

void VulkanContext::copyBuffersToGPU(
    const std::vector<CopyBufferInfo> &bufferInfos) {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(m_uploadContext.commandBuffer, &beginInfo);

  for (auto bufferInfo : bufferInfos) {
    VkBuffer srcBuffer = bufferInfo.srcBuffer;
    VkBuffer dstBuffer = bufferInfo.dstBuffer;
    VkDeviceSize size = bufferInfo.size;

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(m_uploadContext.commandBuffer, srcBuffer, dstBuffer, 1,
                    &copyRegion);
  }
  vkEndCommandBuffer(m_uploadContext.commandBuffer);
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &m_uploadContext.commandBuffer;

  vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_uploadContext.fence);
  vkWaitForFences(m_device, 1, &m_uploadContext.fence, VK_TRUE, UINT64_MAX);
  vkResetCommandBuffer(m_uploadContext.commandBuffer, 0);

  vkResetFences(m_device, 1, &m_uploadContext.fence);
}

VulkanContext::~VulkanContext() {
  destroyRenderPass();
  destroyUploadContext();
  destroySyncObjects();
  destroySwapChain();

  vkDestroyCommandPool(m_device, m_commandPool, nullptr);

  vmaDestroyAllocator(m_allocator);
  vkDestroyDevice(m_device, nullptr);
  vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
  vkDestroyInstance(m_instance, nullptr);
}

} // namespace Renderer