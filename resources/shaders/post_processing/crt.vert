#version 330 core

layout(location = 0) in vec3 v_position;
layout(location = 2) in vec2 v_uv;

uniform mat4 u_model;

out vec2 f_uv;

void main() {
    gl_Position = vec4(v_position, 1.0f);

    f_uv = v_uv;
}
