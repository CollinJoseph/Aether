set(PCH_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/pch/pch.hpp")

add_library(Aether STATIC
        engine/engine.cpp
        engine/window.cpp
        renderer/renderer.cpp
        renderer/vulkanContext.cpp
)

target_include_directories(Aether PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_precompile_headers(Aether PRIVATE
        ${PCH_HEADER}
)

find_package(Vulkan REQUIRED COMPONENTS glslc)
find_package(glfw3 REQUIRED)

target_link_libraries(Aether PRIVATE Vulkan::Vulkan glfw)

add_dependencies(Aether compile_shaders)