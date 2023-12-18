#include "RayTraceVoxel.h"

glm::vec3 fRadiance;

// Random seed generation for random functions
glm::vec2 seedGen(glm::vec2 fragCoord) {
	time_t seconds;
	seconds = time(NULL);
	float r1 = fragCoord.x + seconds;					// TODO: add gl_fragCoord.x + seconds
	float r2 = fragCoord.y + sin(float(seconds));     // TODO: add gl_fragCoord.y + sin( float(iFrame) );

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

// Imported from CPSC 591 - A2 with openGL modifications
// This function creates a local frame (tangent, bitangent, normal) based 
// on a given normal vector 'N'.
// Parameters:
// - n: The normal vector around which to create the local frame.
// - T: An 'out' parameter to store the tangent vector.
// - B: An 'out' parameter to store the bitangent vector.
//It calculates and sets 'T' and 'B'. returns T&B in a vector
std::vector<glm::vec3> createLocalFrame(glm::vec3& n, glm::vec3& T, glm::vec3& B) {
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
// This function samples a point on a hemisphere with a cosine-weighted 
// distribution based on 
// the provided normal vector 'N' and random values 'r1' and 'r2'.
//
// Parameters:
// - n: The normal vector around which the hemisphere is oriented.
// - r1: A random value used for azimuthal angle calculation.
// - r2: A random value used for polar angle calculation.
//
// Returns:
// - A 3D vector representing the sampled direction in world space.
glm::vec3 SampleHemisphere(glm::vec3& n, float r1, float r2) {
	float phi = r1 * 2.0 * M_PI;     // phi is in [0,360)   // Azimuthal Angle
	float cosTheta = sqrt(1.0 - r2); // theta is in [0,180] // Polar Angle
	//float sineTheta = sqrt(r2) // Uniformly distributed sine
	float sinTheta = sqrt(1.0 - (cosTheta * cosTheta)); // Cosine weighted sine

	float x = sinTheta * cos(phi);
	float y = sinTheta * sin(phi);
	float z = cosTheta;

	glm::vec3 T = glm::vec3(0);
	glm::vec3 B = glm::vec3(0);
	std::vector<glm::vec3> TBList = createLocalFrame(n, T, B);
	T = TBList.at(0);
	B = TBList.at(1);
	glm::vec3 Va = T * x;
	glm::vec3 Vb = B * y;
	glm::vec3 Vc = n * z;

	glm::vec3 sampledDirection = Va + Vb + Vc;

	return normalize(sampledDirection);
}

// Imported from CPSC 591 - A2 with openGL modifications
void Material_Diffuse(
	Ray& ray,				   // The current ray to be modified.
	glm::vec3& intersectPoint,  // The intersection point in world space.
	glm::vec3& n,               // The normal at the intersection point.
	float r1,                  // A random value for polar angle.
	float r2,                  // A random value for azimuthal angle.
	glm::vec3& fAcc,            // Accumulated reflectance factor.
	glm::vec3& emission,        // The emissiveness of the object at the point
	glm::vec3& finalCol)        // The final radiance color to be modified.
{  

	// if the dot product of n and ray.direction is negative, flip the normal
	if (glm::dot(n, ray.direction) > 0.0) n *= -1.0;

	// Passes corrected normal to sample a new hemisphere 
	glm::vec3 sampledDirection = SampleHemisphere(n, r1, r2);

	// Alter the ray direction with the new sample
	ray.direction = sampledDirection;
	ray.origin = intersectPoint;

	// fAcc used a factor of how much emission is contributed by the intersected object
	finalCol += fAcc * emission;
	//finalCol += fAcc * obj.emission
}

// Imported from CPSC 591 - A2 with openGL modifications
void Material_Specular_Glossy(
	Ray& ray,							// The current ray to be modified.
	glm::vec3& intersectPoint,           // The intersection point in world space.  
	glm::vec3& n,                        // The normal at the intersection point.
	glm::vec3& fAcc,                     // Accumulated reflectance factor.
	glm::vec3& emission,                 // The emissiveness of the object at the point
	glm::vec3& finalCol)                 // The final radiance color to be modified.
{
	
	ray.direction = reflect(ray.direction, n);   //reflect the ray
	ray.origin = intersectPoint;                 // setting the new origin of the ray
	//finalCol += fAcc * obj.emission;           //Accumulate the emission of the final colour
	finalCol += fAcc * emission;
}

// Imported from CPSC 591 - A2 with openGL modifications
void Material_Specular_Glossy_Transparent(
	Ray& ray,						// Current ray to be modified.
	glm::vec3 &intersectPoint,       // Intersection point in world space.
	glm::vec3 &n,                    // Normal at the intersection point.
	glm::vec2 &seed,                 // Random number generator seed.
	glm::vec3 &fAcc,                 // Accumulated reflectance factor.
	glm::vec3& emission,             // The emissiveness of the object at the point
	glm::vec3& finalCol)             // Final radiance color to be modified.
{ 

	//Ray reflRay = Ray(p, ray.direction - n * 2.0 * dot(n, ray.direction));
	Ray reflRay;
	reflRay.origin = intersectPoint;
	reflRay.direction = ray.direction - n * 2.f * dot(n, ray.direction);

	//glm::vec3 nl = dot(n, ray.direction) < 0.0 ? n : n * -1.0;
	glm::vec3 nl = dot(n, ray.direction) < 0.f ? n : n * -1.f;
	bool into = dot(n, nl) > 0.0;  // Checks if the ray is entering the object.

	float nc = 1.0;  // IOR of air
	float nt = 1.5;  // IOR of solid
	float nnt = into ? nc / nt : nt / nc; // Calculate the ratio of refractive indices.
	float ddn = dot(ray.direction, nl); // Calculate the dot product between ray direction and normal.
	float cos2t = 1.0 - nnt * nnt * (1.0 - ddn * ddn); // Calculate the squared cosine of the transmitted angle.

	// Total internal reflection?
	// If cos2t is negative, no refraction is possible.
	if (cos2t < 0.0)
	{
		ray = reflRay; // Set the ray to the reflected ray.
		//finalCol = finalCol + fAcc * obj.emission; // Accumulate emission color.
		finalCol = finalCol + fAcc * emission; // Accumulate emission color.
	}
	else
	{
		// Adjust the intoFactor based on ray direction.
		float intoFactor = into ? 1.0 : -1.0;

		// Calculate the direction of the transmitted ray (refraction).

		// Calculate the offset direction used to compute tdir.
		float offsetDir = intoFactor * (ddn * nnt + sqrt(cos2t));
		// Calculate the new direction (nntDir) after refraction.
		glm::vec3 nntDir = ray.direction * nnt - n * offsetDir;
		// Normalize the new direction to get the final refraction direction (tdir).
		glm::vec3 tdir = normalize(nntDir);

		// Calculate the difference in refractive indices.
		float a = nt - nc;
		// Calculate the sum of refractive indices.
		float b = nt + nc;
		// Compute the Fresnel reflection coefficient (unpolarized).
		float R0 = a * a / (b * b);

		// Calculate the costheta value, which indicates the angle between the normal and the ray direction.
		// The 'into' condition determines whether the ray is entering or leaving the object.
		// If it's entering, negate the dot product (cosine of the angle) with the normal.
		float c = 1.0 - (into ? -ddn : dot(tdir, n));

		// Calculate the reflection coefficient for Fresnel equations.
		// It combines the Fresnel-Schlick approximation with the costheta value.
		// R0 is the reflectance at normal incidence, and c represents the costheta term.
		float c5 = c * c * c * c * c;
		float Re = R0 + (1.0 - R0) * c5; // Fresnel reflection coefficient

		// Calculate the transmission coefficient. It's complementary to the reflection coefficient.
		// It represents the portion of light that is transmitted or refracted.
		float Tr = 1.0 - Re; // Transmission coefficient

		// Calculate the probability of reflection (P) based on the reflectance (Re).
		// 0.25 is a typical value for the probability of specular reflection.
		float P = 0.25 + 0.5 * Re;

		// Calculate the reflection factor. It's the ratio of reflectance to the probability of reflection.
		float RP = Re / P;

		// Calculate the transmission factor. It's the ratio of transmittance to the probability of transmission.
		float TP = Tr / (1.0 - P);

		// Generate a random number between 0.0 and 1.0.
		seed.y = rand(seed); // (...student code...) seeding the random number - showed by Xaoloi

		//(...student code start...)
		if (seed.y < P)
		{
			// Probability-based decision: 
			// Perform REFLECTION.

			ray.direction = reflect(ray.direction, n);
			ray.direction = normalize(ray.direction);
			ray.origin = intersectPoint;
			//finalCol += fAcc * obj.emission * RP;
			finalCol += fAcc * emission * RP;

		}
		else
		{
			// robability-based decision: 
			// Perform REFRACTION / Transmission.

			ray.direction = tdir;      // refraction direction b?
			ray.direction = normalize(ray.direction);
			ray.origin = intersectPoint;           
			//finalCol += fAcc * obj.emission * TP;
			finalCol += fAcc * emission * TP;

		}
		// One possible change is putting *TP in finalCol instead
	}
}

void apply_BRDF(
	Ray &ray,					// The current ray to be modified.
	glm::vec3 &intersectPoint,   // Intersection point.
	glm::vec3 &n,				// Normal at the intersection point.
	float r1,					// Corrected normal direction.
	float r2,					// Random value for BRDF.
	glm::vec2 &seed,				// Random seed for BRDF.
	glm::vec3 &k,				// Vector of kd, ks, kt
	glm::vec3 &fAcc,				// Accumulated reflectance factor.
	glm::vec3 &emission,		    // The emissiveness of the object at the point
	glm::vec3 &finalCol)			// Final radiance color.
{

	// Corrected normal direction
	glm::vec3 nl = dot(n, ray.direction) < 0.f ? n : n * -1.f;
	//glm::vec3 nl = glm::reflect(n, ray.direction);

	// kd = Diffuse Reflectance Coefficient
	// ks = Specular Reflectance Coefficient
	// kt = Transmittance Coefficient
	// TODO: sample kd,ks,kt from object - currently sampling from panel to test;
	//float kd = panel::kd;
	//float ks = panel::ks;
	//float kt = panel::ka;

	//float kd = 0.0;
	//float ks = 0.0;
	//float kt = 1.0;

	//// TODO: will need to remove this hardcode if sampling from each object
	//// Hardcode that if the emission of the object is greater than 0 - meaning it's a light
	//// set the kd,ks,and kt to appropriate light values found in CPSC 591 - A2
	//if (emission.x > 0.f || emission.y > 0.f || emission.z > 0.f) {
	//	kd = 1.0;
	//	ks = 0.0;
	//	kt = 0.0;
	//}

	//float ktot = kd + ks + kt; // Calculating the total reflectance factor
	float ktot = k.x + k.s + k.z;
	float R = r1 * ktot;       // Scaling a random number in the range (0.0, ktot)
	if (R < k.x) {
		//std::cout << "Diffuse" << std::endl;
		Material_Diffuse(ray, intersectPoint, n, r1, r2, fAcc, emission, finalCol);
	}
	else {
		if (R < k.x + k.y) {
			//std::cout << "Specular Glossy" << std::endl;
			Material_Specular_Glossy(ray, intersectPoint, n, fAcc, emission, finalCol);
		}
		else {
			//std::cout << "Transparent" << std::endl;
			Material_Specular_Glossy_Transparent(ray, intersectPoint, n, seed, 
				fAcc, emission, finalCol);
		}
	}
}

// Function to check if there's an interesction with the opal voxel grid
glm::vec3 IntersectGrid(Ray &ray, VoxelGrid<clusterData> &vGrid) {
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
	glm::vec3 marchAmount = normalize(ray.direction);
	// Marches along the ray in quarter increments to find a voxel intersection
	// (voxels are unit size)
	for (int i = 0.f; i < maxVGridDimension; i ++) {
		// If the ray march position is in the positives (the voxel grid is all positive)
		// and is within the voxel grid dimensions
		if (marchPoint.x >= 0.f && marchPoint.x < vGrid.getDimensions().x
			&& marchPoint.y >= 0.f && marchPoint.y < vGrid.getDimensions().y
			&& marchPoint.z >= 0.f && marchPoint.z < vGrid.getDimensions().z) {

			//std::cout << "March point: " << marchPoint.x << "," << marchPoint.y << ","
			//	<< marchPoint.z << std::endl;

			// check what material of the voxel the ray has intersected with
			// if it's a cluster
			if (vGrid.at((int)marchPoint.x, (int)marchPoint.y, (int)marchPoint.z).material == Cluster) {
				//std::cout << "March point hit cluster: " << 
				//	(int)marchPoint.x << "," << (int)marchPoint.y << ","
				//	<< (int)marchPoint.z  << std::endl;
				
				// if the ray has intersected with a cluster, return the ray march point
				return marchPoint;
			}
		}
		marchPoint += marchAmount; // Increment the current march point
	}
	// if no intersection was found, return a glm::vec3 of -1 
	return marchPoint = glm::vec3(-1);
}

// function to check if there's an intersection with the light
glm::vec3 IntersectLight(Ray &ray) {
	glm::vec3 marchPoint = ray.origin;
	glm::vec3 marchAmount = normalize(ray.direction);

	float marchLimit = 100.f; // the limit of how many times the ray should march
	float proximityDif = 1.f; // what the diff should be between current ray point and lightPos

	// march the ray
	for (float i = 0.f; i < marchLimit; i += 0.25f) {
		// if the ray is within the range of the light, return that point
		if (marchPoint.x - panel::lightPos.x <= proximityDif &&
			marchPoint.y - panel::lightPos.y <= proximityDif &&
			marchPoint.z - panel::lightPos.z <= proximityDif) {
			return marchPoint;
		}
		marchPoint += marchAmount; // Increment the current march point
	}
	// if no intersection is found, return glm::vec3 of -1000
	return marchPoint = glm::vec3(-1000);
}

glm::vec3 Intersect(
	Ray& ray, 
	VoxelGrid<clusterData>& vGrid, 
	glm::vec3& fAcc, 
	glm::vec3& emission,
	glm::vec3& n)
{
	// Finds the largets dimension of the vGrid to set as the ray march limit
	int maxVGridDimension = vGrid.getDimensions().x;
	if (maxVGridDimension < vGrid.getDimensions().y) maxVGridDimension = vGrid.getDimensions().y;
	if (maxVGridDimension < vGrid.getDimensions().z) maxVGridDimension = vGrid.getDimensions().z;

	glm::vec3 marchPoint = ray.origin;
	glm::vec3 marchAmount = normalize(ray.direction);

	int marchCountNoHit = 0;
	int marchCountNoHitMax = 200;
	float proximityDif = 1.f; // what the diff should be between current ray point and lightPos

	while(marchCountNoHit < marchCountNoHitMax) {		

		//std::cout << "March Point: " << marchPoint.x << "," << marchPoint.y << "," << marchPoint.z << std::endl;

		// If the ray march position is in the positives (the voxel grid is all positive)
		// and is within the voxel grid dimensions
		if (marchPoint.x >= 0.f && marchPoint.x < vGrid.getDimensions().x
			&& marchPoint.y >= 0.f && marchPoint.y < vGrid.getDimensions().y
			&& marchPoint.z >= 0.f && marchPoint.z < vGrid.getDimensions().z) {
			
			if (vGrid.at((int)marchPoint.x, (int)marchPoint.y, (int)marchPoint.z).material == Cluster) {
				//std::cout << "March point hit cluster: " << 
				//	(int)marchPoint.x << "," << (int)marchPoint.y << ","
				//	<< (int)marchPoint.z  << std::endl;

				float n_x = marchPoint.x - (int)marchPoint.x;
				float n_y = marchPoint.y - (int)marchPoint.y;
				float n_z = marchPoint.z - (int)marchPoint.z;
				n = normalize(glm::vec3(n_x,n_y,n_z));

				// Updates the accumulation by multiplying the objects colour
				fAcc *= vGrid.at((int)marchPoint.x,
				(int)marchPoint.y,
				(int)marchPoint.z).incLightWavelength.at(0);

				// No emission for objects
				emission = glm::vec3(0);

				float kd = 0.0;
				float ks = 0.0;
				float kt = 1.0;

				// if the ray has intersected with a cluster, return the ray march point
				return marchPoint;
			}

		}
		else if (marchPoint.x - panel::lightPos.x <= proximityDif &&
			marchPoint.y - panel::lightPos.y <= proximityDif &&
			marchPoint.z - panel::lightPos.z <= proximityDif) {

			//std::cout << "March Point hit Light: " << marchPoint.x << ","
			//	<< marchPoint.y << "," << marchPoint.z << std::endl;

			float n_x = marchPoint.x - panel::lightPos.x;
			float n_y = marchPoint.y - panel::lightPos.y;
			float n_z = marchPoint.z - panel::lightPos.z;
			n = normalize(glm::vec3(n_x, n_y, n_z));

			// White light
			fAcc *= glm::vec3(1);
			emission = glm::vec3(panel::light_emission);

			float kd = 1.0;
			float ks = 0.0;
			float kt = 0.0;

			return marchPoint;
		}
		//std::cout << "March iteration: " << marchCountNoHit << std::endl;
		marchCountNoHit++;
		marchPoint += marchAmount; // Increment the current march point
	}
	// if no intersection was found, return a glm::vec3 of -1 
	//std::cout << "no hit" << std::endl;
	return marchPoint = glm::vec3(-1000);
}

// Iterates through every single object in the scene and finds the closest hit distance
// assumes every object is a sphere
bool wholeSceneIntersect(
	Ray& ray, 
	VoxelGrid<clusterData>& vGrid,
	glm::vec3 objPos,
	float& d,
	glm::vec3& emission,
	glm::vec3& fAcc,
	glm::vec3& k) 
{
	// Variable to hold closest distance
	d = 0;

	// Infinite value for the intersection distance.
	float inf = std::numeric_limits<float>::max();

	// t to the inital 'ifinite' value
	float t = inf; // TO BE RETURNED 

	// Checking light intersect distance
	//d = SphereIntersect(ray, panel::lightPos, 1.f);
	d = voxelGridIntersect(ray, vGrid);

	if (d > 0) {
		t = d;
		objPos = panel::lightPos;
		emission = glm::vec3(1.0); // emissive
		fAcc *= glm::vec3(1.0);    // white
		k = glm::vec3(1.0, 0.0, 0.0); // diffuse
	}

	bool isValid = d > 0;

	return isValid;
}



// Function to calculate final radiance 
glm::vec3 CalculateRadiance(Ray &ray, glm::vec2 seed, 
	int max_path_length, 
	VoxelGrid<clusterData> &vGrid) {

	glm::vec3 finalCol = glm::vec3(0);   // Initialize final colour to black
	glm::vec3 fAcc = glm::vec3(1.0);     // Initialize final accumulated reflectance factor to white.
	glm::vec3 emission = glm::vec3(0);
	glm::vec3 n = glm::vec3(0);
	glm::vec3 k = glm::vec3(0);			// vector of kd, ks, kt

	for (int i = 0; i < max_path_length; i++) {
		float hitDist = 0;
		glm::vec3 objPos = glm::vec3(0);
		if (wholeSceneIntersect(ray, vGrid, objPos, hitDist,emission,fAcc,k)) {
			// generating random numbers for the BRDF
			time_t seconds;
			seconds = time(NULL);
			float r1 = rand(seed);
			seed.x = sin(r1 - float(seconds));
			float r2 = rand(seed);
			seconds = time(NULL);
			seed.y = sin(r2 + float(seconds));

			glm::vec3 intersectPoint = ray.origin + ray.direction * hitDist;
			glm::vec3 n = normalize(intersectPoint - objPos);

			apply_BRDF(ray, intersectPoint, n, r1, r2, seed, k, fAcc, emission, finalCol);
		}

	}
	return finalCol;
}

// Main function for ray tracing - with a ray already passed and calculated for each pixel
glm::vec3 RayTraceVoxelV2(
	RayAndPixel r,
	int sample_count,
	int max_path_length,
	VoxelGrid<clusterData>& vGrid) {

	fRadiance = glm::vec3(0); // clear the radiance vector

	for (int i = 0; i != sample_count; i++) {
		fRadiance += CalculateRadiance(r.ray, seedGen(glm::vec2(r.x,r.y)), max_path_length, vGrid);
	}

	// Compute the average radiance over the samples
	fRadiance /= float(sample_count);

	// gamma correction
	const float gamma = 2.2;
	fRadiance = pow(fRadiance, glm::vec3(1.0 / gamma));
	return fRadiance ;
}

// Assigns a pixel with a ray direction 
std::vector<RayAndPixel> getRaysForViewpoint(ImageBuffer& image, Camera& cam) {
	std::vector<RayAndPixel> rays;
	float aspectRatio = (float)image.Width() / image.Height();

	float planeHeight = 0.1 * tan(glm::radians(45.f) * 0.5) * 2;
	float planeWidth = planeHeight * aspectRatio;
	glm::vec3 bottomLeftLocal = glm::vec3(-planeWidth / 2, -planeHeight / 2, 0.1);

	glm::vec3 camRight = glm::normalize(glm::cross(cam.getDir(), cam.getUp()));

	//x,y relate to the x,y of the image output pixels. This loop iterates through each pixel on the screen
	for (int x = 0; x < image.Width(); x++) {
		for (int y = 0; y < image.Height(); y++) {
			float tx = x / ((float)image.Width() - 1);
			float ty = y / ((float)image.Height() - 1);


			glm::vec3 pointLocal = bottomLeftLocal + glm::vec3(planeWidth * tx, planeHeight * ty, 0);
			glm::vec3 point = cam.getPos() + camRight * pointLocal.x + cam.getUp() * pointLocal.y + cam.getDir() * pointLocal.z;
			glm::vec3 rayDirection = glm::normalize(point - cam.getPos());

			Ray r = Ray(cam.getPos(), rayDirection);
			rays.push_back({ r, x, y });
		}
	}
	return rays;
}

// iterates through all of the rays pixel pairs and traces those paths,
// then assigns them to an image
void rayTraceImage(
	ImageBuffer& image, 
	Camera& cam,
	int sample_count,
	int max_path_length,
	VoxelGrid<clusterData>& vGrid) 
{
	image.Initialize();
	std::vector<RayAndPixel> rays = getRaysForViewpoint(image, cam);

	int count = 0;
	for (auto const& r : rays) {
		count++;
		if (count % 5000 == 0) {
			std::cout << ((float)count * 100) / (image.Width() * image.Height()) << "%\n";
		}
		glm::vec3 color = RayTraceVoxelV2(r, sample_count, max_path_length, vGrid);
		image.SetPixel(r.x, r.y, color);
		//std::cout << "colour: " << color.x << "," << color.y << "," << color.z << std::endl;
	}
}

// returns a random pixel in the window space
glm::vec2 getRandPixel(glm::vec2 &windowSize) {
	glm::vec2 pixels;

	std::random_device rd; // obtain a random number from hardware
	std::mt19937 gen(rd()); // seed the generator
	std::uniform_int_distribution<> distrx(0, windowSize.x); // define the range

	pixels.x = (distrx(gen));

	std::uniform_int_distribution<> distry(0, windowSize.y); // define the range

	pixels.y = (distry(gen));

	return pixels;
}
