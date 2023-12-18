#version 410

layout(location = 0) out vec4 vFragColor;
in vec4 set_color;

void main() {
    vFragColor = set_color;
}

