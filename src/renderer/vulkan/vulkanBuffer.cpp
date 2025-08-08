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

VulkanBuffer::VulkanBuffer(VulkanBuffer &&other) noexcept
    : m_allocator(other.m_allocator), // copy: allocators are typically okay
      m_buffer(other.m_buffer),       // take ownership
      m_allocation(other.m_allocation),
      m_allocationInfo(other.m_allocationInfo),
      m_isPersistentlyMapped(other.m_isPersistentlyMapped) {
  // Reset 'other' so its destructor doesn't destroy your resource
  other.m_buffer = VK_NULL_HANDLE;
  other.m_allocation = VK_NULL_HANDLE;
  other.m_isPersistentlyMapped = false;
  std::memset(&other.m_allocationInfo, 0, sizeof(VmaAllocationInfo));
}

VulkanBuffer &VulkanBuffer::operator=(VulkanBuffer &&other) noexcept {
  if (this != &other) {
    // 1. Release any resource currently held by *this to avoid leaks
    if (m_buffer != VK_NULL_HANDLE && m_allocation != VK_NULL_HANDLE) {
      vmaDestroyBuffer(m_allocator, m_buffer, m_allocation);
    }

    // 2. Transfer ownership of resources from other to *this
    m_allocator = other.m_allocator;
    m_buffer = other.m_buffer;
    m_allocation = other.m_allocation;
    m_allocationInfo = other.m_allocationInfo;
    m_isPersistentlyMapped = other.m_isPersistentlyMapped;

    // 3. Leave other in a valid empty state
    other.m_buffer = VK_NULL_HANDLE;
    other.m_allocation = VK_NULL_HANDLE;
    other.m_isPersistentlyMapped = false;
    std::memset(&other.m_allocationInfo, 0, sizeof(VmaAllocationInfo));
  }
  return *this;
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