#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_norm;

//instanced 
layout (location = 2) in mat4 instanceMatrix; //uses 2,3,4,5
layout (location = 6) in vec3 orientation;
layout (location = 7) in int material;

uniform mat4 cameraMat;

uniform vec3 in_center;

out vec3 norm;
out vec3 cube_norm;
out vec3 FragPos;
out vec3 out_orientation;
flat out int vert_mat;

//vec3 lightDir = normalize(vec3(1.3,2.3,4));

mat3 eulerToMatrix(vec3 angles) {
    float c1 = cos(angles.z); // yaw
    float s1 = sin(angles.z);

    float c2 = cos(angles.x); // pitch
    float s2 = sin(angles.x);

    float c3 = cos(angles.y); // roll
    float s3 = sin(angles.y);

    mat3 rotationMatrix;

    rotationMatrix[0][0] = c1 * c3 - s1 * s2 * s3;
    rotationMatrix[0][1] = -c2 * s3;
    rotationMatrix[0][2] = s1 * c3 + s2 * s3 * c1;

    rotationMatrix[1][0] = c1 * s3 + s1 * s2 * c3;
    rotationMatrix[1][1] = c2 * c3;
    rotationMatrix[1][2] = s1 * s3 - s2 * c1 * c3;

    rotationMatrix[2][0] = -c1 * s2;
    rotationMatrix[2][1] = s2;
    rotationMatrix[2][2] = c1 * c2;

    return rotationMatrix;
}

void main() {
    vec3 newOrient = orientation;
    cube_norm = in_norm;
    for(int i = 0; i < 4; i++) {
        newOrient.x = newOrient.x > 45.f ? newOrient.x - 90.f : newOrient.x;
        newOrient.y = newOrient.y > 45.f ? newOrient.y - 90.f : newOrient.y;
        newOrient.z = newOrient.z > 45.f ? newOrient.z - 90.f : newOrient.z;
    }
    newOrient *= 3.14/180.f; //convert to rads
    mat4 rotation = mat4(eulerToMatrix(newOrient));
	//norm = rotation * in_norm;
    norm = mat3(transpose(inverse(rotation))) * normalize(in_pos - in_center);
	FragPos = vec3(instanceMatrix * vec4(in_pos, 1.0));
	gl_Position = cameraMat * instanceMatrix * vec4(in_pos, 1.0);
	out_orientation = orientation;
    vert_mat = material;
}

