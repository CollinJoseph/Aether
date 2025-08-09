#version 450

layout (binding = 0) uniform UniformBufferObject {
    mat4 proj;
    mat4 view;
} ubo;

layout (std430, binding = 1) buffer readonly TransformBuffer {
    mat4 matrices[];
};

layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec3 inColor;

layout (location = 0) out vec3 fragColor;

void main() {
    gl_Position = ubo.proj * ubo.view * matrices[gl_InstanceIndex] * vec4(inPosition, 0.0, 1.0);
    fragColor = inColor;
}