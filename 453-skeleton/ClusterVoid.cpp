#include "VoxelGrid.h"
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <vector>

struct clusterData{
	std::vector<glm::vec3> incLightWavelength;	// Wavelength (colour) of incoming light
	std::vector<glm::vec3> incLightAmplitude;	// Amplitude (energy) of incoming light
	std::vector<glm::vec3> incLightDir;			// std vector because multiple incoming lights
	glm::vec3 orientation;						// Orientation of voxel / cluster (euler angle difference from the up vector)
	bool isVoid;								// Boolean if the voxel / cluster is a void
};

// Used to find the average of a vector
//https://www.techiedelight.com/find-average-of-all-values-present-in-a-vector-in-cpp/
template<typename T>
double getAverage(std::vector<T> const& v) {
	if (v.empty()) {
		return 0;
	}
	return std::accumulate(v.begin(), v.end(), 0.0) / v.size();
}

void setupGrid(int x, int y, int z) {
	VoxelGrid<clusterData> vGrid(x, y, z);     //set up the entire cluster structure as x,y,z dimension
}

void distributeVoidClusters(int x, int y, int z, VoxelGrid<clusterData>& vGrid) {
	// Step 1 - initializes every cluster as a void
	for (int i = 0; i < vGrid.getDimensions().x; i++) {
		for (int j = 0; j < vGrid.getDimensions().y; j++) {
			for (int k = 0; k < vGrid.getDimensions().z; k++) {
				vGrid.at(i, j, k).isVoid = true;
			}
		}
	}

	// selects cells at random 
	int current_x = glm::linearRand<int>(0, vGrid.getDimensions().x);
	int current_y = glm::linearRand<int>(0, vGrid.getDimensions().y);
	int current_z = glm::linearRand<int>(0, vGrid.getDimensions().z);
	clusterData& currCluster = vGrid.at(current_x, current_y, current_z);
	// sets it as a cluster
	currCluster.isVoid = false;

	// sets the rotation randomly
	float randOrientation_x = glm::linearRand<float>(0, 360);
	float randOrientation_y = glm::linearRand<float>(0, 360);
	float randOrientation_z = glm::linearRand<float>(0, 360);
	glm::vec3 randOrientation = glm::vec3(randOrientation_x, randOrientation_y, randOrientation_z);
	currCluster.orientation = randOrientation;
}

// 26 neighbours
void setNeighbours(int x, int y, int z, VoxelGrid<clusterData>& vGrid, glm::vec3 randOrientation) {

	if (x - 1 >= 0 && x - 1 <= vGrid.getDimensions().x) {
		vGrid.at(x - 1, y, z).isVoid = false;
		vGrid.at(x - 1, y, z).orientation = randOrientation;
	}
	if (x + 1 >= 0 && x + 1 <= vGrid.getDimensions().x) {
		vGrid.at(x + 1, y, z).isVoid = false;
		vGrid.at(x + 1, y, z).orientation = randOrientation;
	}

	if (y - 1 >= 0 && y - 1 <= vGrid.getDimensions().y) {
		vGrid.at(x, y - 1, z).isVoid = false;
		vGrid.at(x, y - 1, z).orientation = randOrientation;
	}
	if (y + 1 >= 0 && y + 1 <= vGrid.getDimensions().y) {
		vGrid.at(x, y + 1 , z).isVoid = false;
		vGrid.at(x, y + 1, z).orientation = randOrientation;
	}

	if (z - 1 >= 0 && z - 1 <= vGrid.getDimensions().z) {
		vGrid.at(x, y, z - 1).isVoid = false;
		vGrid.at(x, y, z - 1).orientation = randOrientation;
	}
	if (z + 1 >= 0 && z + 1 <= vGrid.getDimensions().z) {
		vGrid.at(x, y, z + 1).isVoid = false;
		vGrid.at(x, y, z + 1).orientation = randOrientation;
	}
}

glm::vec3 checkNeighbours(int x, int y, int z, VoxelGrid<clusterData>& vGrid) {
	std::vector<glm::vec3> orientations;

	// Remember to normalize vectors
	// Edge case the result might be a zero vector, in that case ignore

	// TODO: pull out the .isVoid into the if check, or else it might be out of range
	if (x - 1 >= 0 && x - 1 <= vGrid.getDimensions().x && vGrid.at(x - 1, y, z).isVoid == false) {
		orientations.push_back(vGrid.at(x - 1, y, z).orientation);
	}
	if (x + 1 >= 0 && x + 1 <= vGrid.getDimensions().x && vGrid.at(x + 1, y, z).isVoid == false) {
		orientations.push_back(vGrid.at(x + 1, y, z).orientation);
	}

	if (y - 1 >= 0 && y - 1 <= vGrid.getDimensions().y && vGrid.at(x, y - 1, z).isVoid == false) {
		orientations.push_back(vGrid.at(x, y - 1, z).orientation);
	}
	if (y + 1 >= 0 && y + 1 <= vGrid.getDimensions().y && vGrid.at(x, y + 1, z).isVoid == false) {
		orientations.push_back(vGrid.at(x, y + 1, z).orientation);
	}

	if (z - 1 >= 0 && z - 1 <= vGrid.getDimensions().z && vGrid.at(x, y, z - 1).isVoid == false) {
		orientations.push_back(vGrid.at(x, y, z - 1).orientation);
	}
	if (z + 1 >= 0 && z + 1 <= vGrid.getDimensions().z && vGrid.at(x, y, z + 1).isVoid == false) {
		orientations.push_back(vGrid.at(x, y, z + 1).orientation);
	}

	glm::vec3 averagedOrientation;
	if (orientations.size() > 0) {
		for (int i = 0; i < orientations.size(); i++) {
			averagedOrientation += orientations.at(i);
		}
		averagedOrientation / (float) orientations.size();
	}
}


