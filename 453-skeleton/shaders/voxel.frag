#version 330 core
out vec4 color;

in vec3 norm;
in vec3 FragPos;  
in vec3 vertCol;
in float alpha;

void main() {
	if(alpha == 0)
		discard;
	//vec3 lightDir = normalize(vec3(1, 5, 3) - FragPos);
	//color = vec4(vertCol, 1);//vec4(max(0, dot(norm, lightDir)) * vertCol, vertAlpha);
	color = vec4(vertCol,1);
}
