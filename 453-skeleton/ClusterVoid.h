#pragma once

#include "VoxelGrid.h"
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <vector>


#include <iostream>

enum Material { Cluster, Void, Empty };

struct clusterData {
	std::vector<glm::vec3> incLightWavelength;	// Wavelength (colour) of incoming light
	std::vector<glm::vec3> incLightAmplitude;	// Amplitude (energy) of incoming light
	std::vector<glm::vec3> incLightDir;			// std vector because multiple incoming lights
	glm::vec3 orientation = glm::vec3(0.0, 0.0, 0.0); // Orientation of voxel / cluster (euler angle difference from the up vector)
	Material material = Empty;								// Boolean if the voxel / cluster is a void
};

glm::vec3 getClusterRatios(VoxelGrid<clusterData>& vGrid);

VoxelGrid<clusterData> setupGrid(int x, int y, int z);
glm::vec3 randOrientation();
float exponential_random(float mean);
void setAllVoid(VoxelGrid<clusterData>& vGrid);
void distributeVoidClusters(VoxelGrid<clusterData>& vGrid, bool sampleNeighbours);
void distributeVoidClusterV2(VoxelGrid<clusterData>& vGrid, float meanRadius);
void setNeighbours(int x, int y, int z, VoxelGrid<clusterData>& vGrid);
glm::vec3 checkNeighbours(int x, int y, int z, VoxelGrid<clusterData>& vGrid);

std::vector<glm::vec3> ellipsoid(float a, float b, float c);
std::vector<glm::vec3> sphereParameterization(float radius);
glm::vec3 postProcessVec(glm::vec3 vec, float a, float b, float c);
void trimVGrid(VoxelGrid<clusterData>& vGrid);