#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_norm;

//instanced 
layout (location = 2) in mat4 instanceMatrix; //uses 2,3,4,5
layout (location = 6) in vec3 orientation;
layout (location = 7) in int material;

uniform mat4 cameraMat;
uniform vec3 radiance;

out vec3 vertCol;

void main() {
	gl_Position = cameraMat * instanceMatrix * vec4(in_pos, 1.0);
	vertCol = radiance;
}