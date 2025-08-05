#include "vulkanBuffer.hpp"

namespace Aether::Renderer::Vulkan {
VulkanBuffer::VulkanBuffer(VmaAllocator allocator, const VkDeviceSize size,
                           const VkBufferUsageFlags usage,
                           const VmaMemoryUsage memoryUsage,
                           const VmaAllocationCreateFlags createFlags)
    : m_allocator(allocator) {

  VkBufferCreateInfo bufferInfo = {};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;

  VmaAllocationCreateInfo allocationInfo = {};
  allocationInfo.usage = memoryUsage;
  allocationInfo.flags = createFlags;

  if (createFlags & VMA_ALLOCATION_CREATE_MAPPED_BIT) {
    m_isPersistentlyMapped = true;
  }

  if (vmaCreateBuffer(m_allocator, &bufferInfo, &allocationInfo, &m_buffer,
                      &m_allocation, &m_allocationInfo) != VK_SUCCESS) {
    throw std::runtime_error("failed to create Vulkan buffer");
  }
}

VulkanBuffer::~VulkanBuffer() {
  vmaDestroyBuffer(m_allocator, m_buffer, m_allocation);
}

VkBuffer VulkanBuffer::getBuffer() const { return m_buffer; }

VmaAllocation VulkanBuffer::getAllocation() const { return m_allocation; }

VmaAllocationInfo VulkanBuffer::getAllocationInfo() const {
  return m_allocationInfo;
}

void VulkanBuffer::copyDataToBuffer(const void *src, const size_t size) const {
  assert(size <= m_allocationInfo.size);
  if (m_isPersistentlyMapped) {
    memcpy(m_allocationInfo.pMappedData, src, size);
  } else {
    void *dst;
    vmaMapMemory(m_allocator, m_allocation, &dst);
    memcpy(dst, src, size);
    vmaUnmapMemory(m_allocator, m_allocation);
  }
}

} // namespace Aether::Renderer::Vulkan