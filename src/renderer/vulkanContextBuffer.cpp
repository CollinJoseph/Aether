#include "vulkanContext.hpp"

#include <glm/ext/matrix_clip_space.hpp>

namespace Renderer {
AllocatedBuffer
VulkanContext::createBuffer(const VkDeviceSize size,
                            const VkBufferUsageFlags usage,
                            const VmaMemoryUsage memoryUsage) const {
  VkBufferCreateInfo bufferInfo = {};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;

  VmaAllocationCreateInfo allocationInfo = {};
  allocationInfo.usage = memoryUsage;
  allocationInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT |
                         VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
  AllocatedBuffer buffer = {};
  if (vmaCreateBuffer(m_allocator, &bufferInfo, &allocationInfo, &buffer.buffer,
                      &buffer.allocation, &buffer.info) != VK_SUCCESS) {
    throw std::runtime_error("failed to create Vulkan buffer");
  }
  return buffer;
}

void VulkanContext::destroyBuffer(AllocatedBuffer buffer) {
  vmaDestroyBuffer(m_allocator, buffer.buffer, buffer.allocation);
}
} // namespace Renderer