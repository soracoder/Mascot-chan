#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 v_color;

out vec3 vertex_color;

void main() {
	// does not alter the vertices at all
	gl_Position = vec4(pos, 1.0);

	vertex_color = v_color;
}