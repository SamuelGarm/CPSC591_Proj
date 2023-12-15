#include "RayTraceVoxel.h"


glm::vec3 radiance = glm::vec3(0);

// Imported from CPSC 591 - A2 with openGL modifications
std::vector<glm::vec3> createLocalFrame(glm::vec3 n, glm::vec3 T, glm::vec3 B) {
	glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
	T = cross(up, n);
	T = normalize(T);
	B = cross(n, T);

	std::vector<glm::vec3> returnList;
	returnList.push_back(T);
	returnList.push_back(B);

	return returnList;
}

// Imported from CPSC 591 - A2 with openGL modifications
glm::vec3 SampleHemisphere(glm::vec3 n, float r1, float r2) {
	float phi = r1 * 2.0 * M_PI;     // phi is in [0,360)   // Azimuthal Angle
	float cosTheta = sqrt(1.0 - r2); // theta is in [0,180] // Polar Angle
	//float sineTheta = sqrt(r2) // Uniformly distributed sine
	float sinTheta = sqrt(1.0 - (cosTheta * cosTheta)); // Cosine weighted sine

	float x = sinTheta * cos(phi);
	float y = sinTheta * sin(phi);
	float z = cosTheta;

	glm::vec3 T;
	glm::vec3 B;
	std::vector<glm::vec3> TBList = createLocalFrame(n, T, B);
	T = TBList.at(0);
	B = TBList.at(1);
	glm::vec3 Va = T * x;
	glm::vec3 Vb = B * y;
	glm::vec3 Vc = n * z;

	glm::vec3 sampledDirection = Va + Vb + Vc;

	return normalize(sampledDirection);
}

void Material_Diffuse(Ray ray, 
	glm::vec3 intersectPoint, 
	glm::vec3 n,
	float r1,
	float r2,
	glm::vec3 fAcc,
	glm::vec3 finalCol) {

	// if the dot product of n and ray.direction is negative, flip the normal
	if (glm::dot(n, ray.direction) > 0.0) n *= -1.0;

	// Passes corrected normal to sample a new hemisphere 
	glm::vec3 sampledDirection = SampleHemisphere(n, r1, r2);

	// Alter the ray direction with the new sample
	ray.direction = sampledDirection;
	ray.origin = intersectPoint;

	// fAcc used a factor of how much emission is contributed by the intersected object
	//finalCol += fAcc * obj.emission;
}

void Material_Specular_Glossy(Ray ray,
	glm::vec3 intersectPoint,
	glm::vec3 n,
	glm::vec3 fAcc,
	glm::vec3 finalCol) {

}

void Material_Specular_Glossy_Transparent(Ray ray,
	glm::vec3 intersectPoint,
	glm::vec3 n,
	glm::vec2 seed,
	glm::vec3 fAcc,
	glm::vec3 finalCol) {

}

void apply_BRDF(Ray ray, 
	glm::vec3 intersectPoint, 
	glm::vec3 n, 
	float r1,
	float r2,
	glm::vec2 seed,
	glm::vec3 fAcc,
	glm::vec3 finalCol) {

	// Corrected normal direction
	glm::vec3 nl = dot(n, ray.direction) < 0.f ? n : n * -1.f;
	//glm::vec3 nl = glm::reflect(n, ray.direction);

	// kd = Diffuse Reflectance Coefficient
	// ks = Specular Reflectance Coefficient
	// kt = Transmittance Coefficient
	// TODO: sample kd,ks,kt from object - currently hardcoding to test
	float kd = 0.5f;
	float ks = 0.5f;
	float kt = 0.5f;

	float ktot = kd + ks + kt; // Calculating the total reflectance factor
	float R = r1 * ktot;       // Scaling a random number in the range (0.0, ktot)
	if (R < kd) {
		Material_Diffuse(ray, intersectPoint, n, r1, r2, fAcc, finalCol);
	}
	else {
		if (R < kd + ks) {
			Material_Specular_Glossy(ray, intersectPoint, n, fAcc, finalCol);
		}
		else {
			Material_Specular_Glossy_Transparent(ray, intersectPoint, n, seed, fAcc, finalCol);
		}
	}
}

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
			
			// Updates the accumulation by multiplying the objects colour
			fAcc *= vGrid.at((int)intersectPoint.x,
				(int)intersectPoint.y,
				(int)intersectPoint.z).incLightWavelength;

			// generating random numbers for the BRDF
			time_t seconds;
			seconds = time(NULL);
			float r1 = rand(seed);
			seed.x = sin(r1 - float(seconds));
			float r2 = rand(seed);
			seconds = time(NULL);
			seed.y = sin(r2 + float(seconds));

			apply_BRDF(ray, intersectPoint, n, r1, r2, seed, fAcc, finalCol);
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


// Imported from CPSC 591 A2
float rand(glm::vec2 st)
// ----------------------------------------------------------------------
// Generates pseudo-random numbers using Perlin noise.
//
// Parameters:
// - st: A 2D vector used as a seed for the random number generation.
// ----------------------------------------------------------------------
{
	// Use Perlin noise for randomness

	// Calculate a dot product between 
	//the input vector 'st.xy' and a constant 
	// vector 'vec2(12.9898, 78.233)'.
	// This dot product is a key step in generating random-like values.
	float dotProduct = glm::dot(st, glm::vec2(12.9898, 78.233));

	// Calculate the sine of the dot product and 
	// take its fractional part using 'fract'.
	// The sine function introduces non-linearity, adding randomness.
	float sineValue = sin(dotProduct);

	// Multiply the sine value by a large constant '43758.5453' to 
	// increase the range and distribution of values.
	float scaledValue = sineValue * 43758.5453;

	// Return the result, which should be a pseudo-random value in the range [0, 1).
	return glm::fract(scaledValue);
}