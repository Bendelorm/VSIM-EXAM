#version 450
// Binding 0: Shared uniform data
layout(binding = 0) uniform UniformBufferObject
{
    mat4 view;
    mat4 proj;
    vec3 lightPos;
    vec3 viewPos;
} ubo;

// Binding 1: Per-instance data (storage buffer)
layout(binding = 1) readonly buffer InstanceBuffer
{
    mat4 model[];
} instanceBuffer;

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aColor;   //For friction area

layout(location = 0) out vec3 outPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outTexCoords;
layout(location = 3) out vec3 outColor;   //For friction area
layout(location = 4) out vec3 outLightPos;
layout(location = 5) out vec3 outViewPos;

void main()
{
    mat4 model = instanceBuffer.model[gl_InstanceIndex];
    gl_PointSize = 5;

    // Transform to world space
    vec4 worldPos = model * vec4(aPos, 1.0);
    outPos = worldPos.xyz;
    outNormal = normalize(mat3(transpose(inverse(model))) * aNormal);

    outTexCoords = aTexCoords;
    outColor = aColor;
    outLightPos = ubo.lightPos;
    outViewPos = ubo.viewPos;

    gl_Position = ubo.proj * ubo.view * worldPos;
}
