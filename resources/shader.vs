#version 330 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_color;

out vec3 vertex_color;
uniform mat4 mvp;

void main() {
	gl_Position = mvp * vec4(in_pos, 1.0f);
	vertex_color = in_color;
}

// vim: set ft=glsl:
