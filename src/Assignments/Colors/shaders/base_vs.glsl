#version 410

layout(location = 0) in vec4 a_vertex_position;

void main()
{
in vec3 fragColor;  // Input color from the vertex shader

out vec4 finalColor;  // Output color to the framebuffer

void main()
{
    finalColor = vec4(fragColor, 1.0);
}
}