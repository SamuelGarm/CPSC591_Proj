#pragma once
#include "Camera.h"
#include <time.h>
#include <limits>

glm::vec3 radiance;

struct Ray {
	glm::vec3 origin;
	glm::vec3 direction;
};

glm::vec3 CalculateRadiance(Ray ray, glm::vec2 seed, int max_path_length);
void RayTraceVoxel(Camera cam, int sample_count, int max_path_length);
glm::vec2 seedGen();