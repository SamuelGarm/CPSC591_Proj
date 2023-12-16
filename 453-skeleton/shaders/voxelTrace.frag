#version 330 core
out vec4 color;

in vec3 vertCol;

void main() {
	color = vec4(vertCol,1);
}