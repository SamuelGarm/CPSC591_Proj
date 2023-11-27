#include "VoxelGrid.h"
#include <glm/glm.hpp>
#include <vector>

struct clusterData{
	std::vector<glm::vec3> accumulatedLight;
	std::vector<glm::vec3> incLightDir;			// std vector because multiple incoming lights
	glm::vec3 orientation;						// Orientation of cluster
};

void setupGrid() {
	VoxelGrid<int> vGrid(100, 75, 25);
	//vGrid.at(0, 0, 0); // returns the in stored at that location
}

