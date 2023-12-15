#include "RayTraceVoxel.h"

glm::vec3 radiance = glm::vec3(0);

glm::vec3 Intersect(Ray ray, VoxelGrid<clusterData> vGrid) {
	// Initialize a variable to hold the closest itersection distance
	//float d;

	// Set an initial "infinite" value for the intersection distance
	//float inf = std::numeric_limits<float>::max();

	// Finds the largets dimension of the vGrid to set as the ray march limit
	int maxVGridDimension = vGrid.getDimensions().x;
	if (maxVGridDimension < vGrid.getDimensions().y) maxVGridDimension = vGrid.getDimensions().y;
	if (maxVGridDimension < vGrid.getDimensions().z) maxVGridDimension = vGrid.getDimensions().z;
	maxVGridDimension += 10; // Increases the limit by a little bit over the size of the grid as a buffer


	glm::vec3 marchPoint = ray.origin;
	glm::vec3 marchAmount = ray.direction * 0.25f;
	// Marches along the ray in quarter increments to find a voxel intersection
	// (voxels are unit size)
	for (float i = 0.f; i < maxVGridDimension; i += 0.25f) {
		// If the ray march position is in the positives (the voxel grid is all positive)
		// and is within the voxel grid dimensions
		if (marchPoint.x >= 0.f && marchPoint.x < vGrid.getDimensions().x
			&& marchPoint.y >= 0.f && marchPoint.y < vGrid.getDimensions().y
			&& marchPoint.z >= 0.f && marchPoint.z < vGrid.getDimensions().z) {

			// check what material of the voxel the ray has intersected with
			// if it's a cluster
			if (vGrid.at((int)marchPoint.x, (int)marchPoint.y, (int)marchPoint.z).material == Cluster) {
				// if the ray has intersected with a cluster, return the ray march point
				return marchPoint;
			}
		}
		marchPoint += marchAmount; // Increment the current march point
	}
	// if no intersection was found, return a glm::vec3 of -1 
	return marchPoint = glm::vec3(-1);
}

// Function to calculate final radiance 
glm::vec3 CalculateRadiance(Ray ray, glm::vec2 seed, 
							int max_path_length, 
							VoxelGrid<clusterData> vGrid) {

	glm::vec3 finalCol = glm::vec3(0);   // Initialize final colour to black
	glm::vec3 fAcc = glm::vec3(1.0);     // Initialize final accumulated reflectance factor to white.

	for (int i = 0; i != max_path_length; i++) {
		// if an intersection was found (as in the function returned a vlue other than -1)
		glm::vec3 intersectPoint = Intersect(ray, vGrid);
		// if an intersection was found
		if (intersectPoint != glm::vec3(-1)) {
			// Calculating the normal at the intersection point
			// I'm honestly not sure why this works, its what's in CPSC 591 A2 in
			// CalculateRadiance() 
			// vec3 n = normalize(p - obj.position);  // Normal at the intersection point.
			float n_x = intersectPoint.x - (int)intersectPoint.x;
			float n_y = intersectPoint.y - (int)intersectPoint.y;
			float n_z = intersectPoint.z - (int)intersectPoint.z;
			glm::vec3 n = normalize(glm::vec3(n_x,n_y,n_z));
		}
	}
}

// Main function for ray tracing
void RayTraceVoxel(Camera cam, 
				   int sample_count, 
				   int max_path_length,
				   VoxelGrid<clusterData> vGrid) {

	for (int i = 0; i != sample_count; i++) {
		Ray ray;
		ray.origin = cam.getPos();
		ray.direction = normalize(cam.getDir());

		radiance += CalculateRadiance(ray, seedGen(), max_path_length, vGrid);
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