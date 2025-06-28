set(PCH_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/pch/pch.hpp")

add_library(Aether STATIC
        engine/engine.cpp
        engine/window.cpp
        renderer/renderer.cpp
        renderer/vulkanContextInitialization.cpp
        renderer/vulkanContextSwapChain.cpp
        renderer/vulkanContextRenderPass.cpp
        renderer/vulkanContextFrame.cpp
)

target_include_directories(Aether PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(Aether PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_precompile_headers(Aether PRIVATE
        ${PCH_HEADER}
)

include(FetchContent)

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG bf71a834948186f4097caa076cd2663c69a10e1e #refs/tags/1.0.1
)

FetchContent_MakeAvailable(glm)

find_package(Vulkan REQUIRED COMPONENTS glslc)
find_package(glfw3 REQUIRED)

target_link_libraries(Aether PRIVATE Vulkan::Vulkan glfw glm::glm)

add_dependencies(Aether compile_shaders)