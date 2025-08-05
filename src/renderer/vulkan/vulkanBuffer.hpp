#pragma once
#include "pch/pch.hpp"
#include "renderer/vmaUsage.hpp"

namespace Aether::Renderer::Vulkan {

class VulkanBuffer {
public:
  VulkanBuffer(VmaAllocator allocator, VkDeviceSize size,
               VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage,
               VmaAllocationCreateFlags createFlags = 0);
  ~VulkanBuffer();
  void copyDataToBuffer(const void *src, size_t size) const;
  VkBuffer getBuffer() const;
  VmaAllocation getAllocation() const;
  VmaAllocationInfo getAllocationInfo() const;

private:
  VmaAllocator m_allocator;
  VkBuffer m_buffer = VK_NULL_HANDLE;
  VmaAllocation m_allocation = VK_NULL_HANDLE;
  VmaAllocationInfo m_allocationInfo = {};
  bool m_isPersistentlyMapped = false;
};

} // namespace Aether::Renderer::Vulkan