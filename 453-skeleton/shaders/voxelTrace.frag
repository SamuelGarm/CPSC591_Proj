#version 330 core
out vec4 color;

in vec3 norm;
in vec3 FragPos;
in vec3 vertCol;
in float alpha;

void main() {
	//if(alpha == 0)
	//	discard;
	color = vec4(vertCol,1);
}