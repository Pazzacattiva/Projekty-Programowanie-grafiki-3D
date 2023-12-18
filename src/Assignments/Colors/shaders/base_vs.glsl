#version 410

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;

out vec4 set_color;

void main() {
    gl_Position = vec4(in_position, 1.0);
    set_color = vec4(in_color, 1.0);
}
