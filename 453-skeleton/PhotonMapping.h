#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "Photon.h"
#include "ClusterVoid.h"
#include "iostream"
#include "RayTraceVoxel.h"
#include <algorithm>

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
	const glm::vec3 intersection= IntersectGrid(ray, vGrid);
	const clusterData& data = vGrid.at(intersection.x, intersection.y, intersection.z);
	glm::vec3 finalCol = glm::vec3(0);
	for (const Photon& photon : data.photons) {
		finalCol += wavelengthToRGB(photon.wavelength) * 1.f; //1 is a placeholder for intensity
	}
	float maxValue = std::max(finalCol.r, std::max(finalCol.g, finalCol.b));
	if (maxValue > 1) {
		finalCol /= maxValue; //normalize the range
	}
	return finalCol;
}

void tracePhoton(Photon& photon, VoxelGrid<clusterData>& vGrid) {
	int iterations = 100;
	while (iterations > 0) {
		iterations--;
		Ray photonRay = { photon.pos, photon.dir };
		//calculate intersection with the cluster
		glm::vec3 intersection = IntersectGrid(photonRay, vGrid);
		if (intersection.x == -1 && intersection.y == -1 && intersection.z == -1) {
			break;
		}
		clusterData data = vGrid.at(intersection.x, intersection.y, intersection.z);
		data.photons.push_back(photon); //store the incoming direction of the photon
		//rotate the unit axis and find which one has the smallest dot product
		handleIntersection(photon, glm::vec3(1, 0, 0), 900);
	}
}

void handleIntersection(Photon& photon, glm::vec3 surfaceNorm, const int gratingPeriod) {
	//handle only rank 0 reflection for now
	DiffractPhoton(photon, surfaceNorm, gratingPeriod, false, 0);
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
		glm::vec3 axis = glm::cross(surfaceNorm, photonDir);
		surfaceNorm *= -1;
		photon.dir = glm::rotate(surfaceNorm, thetam, axis);
	}
	else {
		glm::vec3 axis = glm::cross(photonDir, surfaceNorm);
		photon.dir = glm::rotate(surfaceNorm, thetam, axis);
	}
}

glm::vec3 calculateDiffractionIrradiance(clusterData cluster, glm::vec3 incomingSampleDir) {
	return glm::vec3(1); //just return white for testing purposes
}

