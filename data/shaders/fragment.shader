#version 330 core

out vec4 out_color;

in vec3 vertex_color;

void main() {

	out_color = vec4(vertex_color, 1.0);

}