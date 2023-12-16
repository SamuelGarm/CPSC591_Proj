#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "Photon.h"
#include "ClusterVoid.h"
#include "iostream"

void handleIntersection(Photon& photon, glm::vec3 surfaceNorm, const int gratingPeriod) {
	//handle only rank 0 for now

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

