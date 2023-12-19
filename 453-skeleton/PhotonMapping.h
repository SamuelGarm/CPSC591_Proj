#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>
#include "Photon.h"
#include "ClusterVoid.h"
#include "iostream"
#include "RayTraceVoxel.h"
#include <algorithm>
#include "intersections.h"

glm::vec3 wavelengthToRGB(float wavelength)
{
	float R = 0.f;
	float G = 0.f;
	float B = 0.f;

	if (wavelength >= 380.f && wavelength <= 440.f) {
		float attenuation = 0.3 + 0.7 * (wavelength - 380) / (440 - 380);
		R = (-(wavelength - 440) / (440 - 380)) * attenuation;
		G = 0.0;
		B = 1.0 * attenuation;
	}
	else if (wavelength >= 440 && wavelength <= 490) {
		R = 0.0;
		G = (wavelength - 440) / (490 - 440);
		B = 1.0;
	}
	else if (wavelength >= 490 && wavelength <= 510) {
		R = 0.0;
		G = 1.0;
		B = -(wavelength - 510) / (510 - 490);
	}
	else if (wavelength >= 510 && wavelength <= 580) {
		R = (wavelength - 510) / (580 - 510);
		G = 1.0;
		B = 0.0;
	}
	else if (wavelength >= 580 && wavelength <= 645) {
		R = 1.0;
		G = -(wavelength - 645) / (645 - 580);
		B = 0.0;
	}
	else if (wavelength >= 645 && wavelength <= 750) {
		float attenuation = 0.3 + 0.7 * (750 - wavelength) / (750 - 645);
		R = 1.0 * attenuation;
		G = 0.0;
		B = 0.0;
	}
	else {
		R = 0.0;
		G = 0.0;
		B = 0.0;
	}

	return glm::vec3(R, G, B);
}

glm::vec3 collectRadiance(Ray ray, VoxelGrid<clusterData>& vGrid) {
	glm::vec3 vGridSize = vGrid.getDimensions();
	Intersection intersection = voxelGridIntersect(ray, vGrid);
	const glm::vec3 intersectPos = intersection.position;
	glm::vec3 toVox = floor(intersection.position);

	while (true) {
		intersection = voxelGridIntersect(ray, vGrid);
		//if the intersection is invalid just break the loop
		if (!intersection.isValid)
			break;

		ray.origin = intersection.position;

		//we know the intersection will be on a plane between 2 voxels
		//calculate the voxel index of the voxel the ray is leaving and the one it is entering over the plane
		glm::vec3 norm = intersection.normal;
		glm::vec3 fromVox = floor(intersection.position);
		toVox = floor(intersection.position);
		if (norm.x == 1 || norm.y == 1 || norm.z == 1) {
			toVox -= norm;
		}
		else {
			fromVox += norm;
		}

		//if the voxel we are entering is outside the boundaries it is not a valid intersection
		if (toVox.x >= vGridSize.x || toVox.x < 0 ||
			toVox.y >= vGridSize.y || toVox.y < 0 ||
			toVox.z >= vGridSize.z || toVox.z < 0) {
			intersection.isValid = false;
			break;
		}


		//loop until we aren't hitting empty space in the voxel grid
		if (vGrid.at(toVox.x, toVox.y, toVox.z).material == Cluster) {
			break;
		}
	}


	glm::vec3 finalCol = glm::vec3(0);
	if (intersection.isValid) {
		
		const clusterData& data = vGrid.at(toVox.x, toVox.y, toVox.z);
		if(data.photons.size() > 0)
			return glm::vec3(1, 0, 0);
			//std::cout << "Cluster has " << data.photons.size() << " Photons\n";
		for (const Photon& photon : data.photons) {
			float angle = acos(glm::dot(photon.dir, -ray.direction));
			float B = (3.1415*panel::particleDiameter*sin(angle)) / (photon.wavelength);
			float intensity = powf(sin(B) / B, 2);
			finalCol += wavelengthToRGB(photon.wavelength) * intensity;
		}
		float maxValue = std::max(finalCol.r, std::max(finalCol.g, finalCol.b));
		if (maxValue > 1) {
			finalCol /= maxValue; //normalize the range
		}
	}
	return finalCol;
}

void DiffractPhoton(Photon& photon, glm::vec3 surfaceNorm, const int gratingPeriod, const bool transmissive, const int rank) {
	glm::vec3 photonDir = -photon.dir; //reverse the photon direction so that it points away from the surface for dot product calculations
	float dirDotNorm = glm::dot(photonDir, surfaceNorm);
	if (dirDotNorm < 0)
		surfaceNorm *= -1.f;
	float incidenceAngle = acos(glm::dot(photonDir, surfaceNorm));
	float frac = (rank * photon.wavelength) / gratingPeriod;
	float b = transmissive ? frac + sin(incidenceAngle) : frac - sin(incidenceAngle);
	//ensure that b is in domain of asin
	if (b < -1 || b > 1) {
		std::cout << "b out of range while calculating refraction\n";
		return;
	}
	float thetam = asin(b);
	if (transmissive) {
		glm::vec3 axis = glm::cross(photonDir, surfaceNorm);
		surfaceNorm *= -1;
		photon.dir = glm::rotate(surfaceNorm, thetam, axis);
	}
	else {
		glm::vec3 axis = glm::cross(surfaceNorm, photonDir);
		photon.dir = glm::rotate(surfaceNorm, thetam, axis);
	}
}

void tracePhoton(Photon& photon, VoxelGrid<clusterData>& vGrid) {

	//add a safely limit so the loop isn't infinite
	int iterations = 100;
	while (iterations > 0) {
		iterations--;
		Ray photonRay = { photon.pos, photon.dir };
		//calculate intersection with the cluster

		glm::vec3 vGridSize = vGrid.getDimensions();
		Intersection intersection = voxelGridIntersect(photonRay, vGrid);
		glm::vec3 toVox = glm::vec3(-1);
		
		
		while (true) {
			photonRay.origin += photonRay.direction * 0.001f;
			intersection = voxelGridIntersect(photonRay, vGrid);
			//if the intersection is invalid just break the loop
			if (!intersection.isValid)
				break;

			photonRay.origin = intersection.position;

			//we know the intersection will be on a plane between 2 voxels
			//calculate the voxel index of the voxel the ray is leaving and the one it is entering over the plane
			glm::vec3 norm = intersection.normal;
			glm::vec3 fromVox = floor(intersection.position);
			toVox = floor(intersection.position);
			if (norm.x == 1 || norm.y == 1 || norm.z == 1) {
				toVox -= norm;
			}
			else {
				fromVox += norm;
			}

			//if the voxel we are entering is outside the boundaries it is not a valid intersection
			if (toVox.x >= vGridSize.x || toVox.x < 0 ||
				toVox.y >= vGridSize.y || toVox.y < 0 ||
				toVox.z >= vGridSize.z || toVox.z < 0) {
				intersection.isValid = false;
				break;
			}


			//loop until we aren't hitting empty space in the voxel grid
			if (vGrid.at(toVox.x, toVox.y, toVox.z).material != Empty) {
				break;
			}
		}

		if (intersection.isValid) {
			clusterData& data = vGrid.at(toVox.x, toVox.y, toVox.z);
			//rotate the unit axis and find which one has the smallest dot product
			//ideally this would choose between rank -1, 0, 1 but for now this works
			DiffractPhoton(photon, intersection.normal, 600, false, 1);
			data.photons.push_back(photon);
		}
		else {
			break;
		}
	}
}

// iterates through all of the rays pixel pairs and traces those paths,
// then assigns them to an image
void collectPhotons(
	ImageBuffer& image,
	Camera& cam,
	int sample_count,
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
		glm::vec3 color = collectRadiance(r.ray, vGrid);
		//glm::vec3 color = RayTraceVoxelV2(r, sample_count, max_path_length, vGrid);
		image.SetPixel(r.x, r.y, color);
		//std::cout << "colour: " << color.x << "," << color.y << "," << color.z << std::endl;
	}
}