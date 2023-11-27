#pragma once

#include "VoxelGrid.h"
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <vector>


#include <iostream>

struct clusterData {
	std::vector<glm::vec3> incLightWavelength;	// Wavelength (colour) of incoming light
	std::vector<glm::vec3> incLightAmplitude;	// Amplitude (energy) of incoming light
	std::vector<glm::vec3> incLightDir;			// std vector because multiple incoming lights
	glm::vec3 orientation = glm::vec3(0.0, 0.0, 0.0); // Orientation of voxel / cluster (euler angle difference from the up vector)
	bool isVoid;								// Boolean if the voxel / cluster is a void
};

void setVoidRatio(float ratio);

VoxelGrid<clusterData> setupGrid(int x, int y, int z);

void distributeVoidClusters(VoxelGrid<clusterData>& vGrid);
void setNeighbours(int x, int y, int z, VoxelGrid<clusterData>& vGrid, glm::vec3 randOrientation);
glm::vec3 checkNeighbours(int x, int y, int z, VoxelGrid<clusterData>& vGrid);