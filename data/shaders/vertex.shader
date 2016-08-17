#version 150

in vec3 pos;

void main() {
	// does not alter the vertices at all
	gl_Position = vec4(pos, 1);
}