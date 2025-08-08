set(PCH_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/pch/pch.hpp")
include(FetchContent)

add_library(Aether STATIC
        engine/engine.cpp
        renderer/window.cpp
        renderer/renderer.cpp
        renderer/vulkan/vulkanDescriptorManager.cpp
        renderer/vulkan/vulkanBuffer.cpp
        renderer/vulkan/vulkanHelpers.cpp
        renderer/vulkanContextInitialization.cpp
        renderer/vulkanContextSwapChain.cpp
        renderer/vulkanContextRenderPass.cpp
        renderer/vulkanContextFrame.cpp
        renderer/vmaUsage.cpp
        resources/resourceLoader.cpp
        ecs/systems/transformSystem.cpp
        ecs/systems/renderSystem.cpp
)

target_include_directories(Aether PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_precompile_headers(Aether PRIVATE
        ${PCH_HEADER}
)

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG bf71a834948186f4097caa076cd2663c69a10e1e #refs/tags/1.0.1
)

FetchContent_Declare(
        entt
        GIT_REPOSITORY https://github.com/skypjack/entt.git
        GIT_TAG d4014c74dc3793aba95ae354d6e23a026c2796db #refs/tags/3.15.0

)

FetchContent_Declare(
        vma
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
        GIT_TAG 1d8f600fd424278486eade7ed3e877c99f0846b1 #refs/tags/3.3.0
)

FetchContent_MakeAvailable(glm vma entt)

find_package(Vulkan REQUIRED COMPONENTS glslc)
find_package(glfw3 REQUIRED)

target_link_libraries(Aether PUBLIC Vulkan::Vulkan glfw glm::glm GPUOpen::VulkanMemoryAllocator)
target_link_libraries(Aether PUBLIC EnTT::EnTT)

add_dependencies(Aether compile_shaders)