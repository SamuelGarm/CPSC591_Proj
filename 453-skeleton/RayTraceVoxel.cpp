#include "RayTraceVoxel.h"

glm::vec3 radiance = glm::vec3(0);

// Function to calculate final radiance 
glm::vec3 CalculateRadiance(Ray ray, glm::vec2 seed, int max_path_length) {
	glm::vec3 finalCol = glm::vec3(0);   // Initialize final colour to black
	glm::vec3 fAcc = glm::vec3(1.0);     // Initialize final accumulated reflectance factor to white.

	for (int i = 0; i != max_path_length; i++) {

	}
}

// Main function for ray tracing
void RayTraceVoxel(Camera cam, int sample_count, int max_path_length) {
	for (int i = 0; i != sample_count; i++) {
		Ray ray;
		ray.origin = cam.getPos();
		ray.direction = normalize(cam.getDir());

		radiance += CalculateRadiance(ray, seedGen(), max_path_length);
	}

	
}

// Random seed generation for random functions
glm::vec2 seedGen() {
	time_t seconds;
	seconds = time(NULL);
	float r1 = seconds;					// TODO: add gl_fragCoord.x + seconds
	float r2 = sin(float(seconds));     // TODO: add gl_fragCoord.y + sin...

	return glm::vec2(r1, r2);
}