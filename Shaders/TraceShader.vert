#version 450

layout(binding = 0) uniform UniformBufferObject
{
    mat4 view;
    mat4 proj;
    vec3 lightPos;
    vec3 viewPos;
} ubo;

// Only position as input: vec3
layout(location = 0) in vec3 aPos;

layout(location = 0) out vec3 outColor;

void main()
{
    // Positions are already in world space
    vec4 worldPos = vec4(aPos, 1.0);

    gl_Position = ubo.proj * ubo.view * worldPos;

    // Constant color for the trace (bright red)
    outColor = vec3(1.0, 0.0, 0.0);
}
