#version 150

out vec4 out_color;

in vec3 vertex_color;

void main() {
	//set every drawn pixel to white
	out_color = vec4(vertex_color, 1.0);
}