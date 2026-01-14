#version 330 core

in vec3 f_colour;

out vec4 o_colour;

void main() {
    o_colour = vec4(f_colour, 1.0f);
}
