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

// 6 neighbours - orthogonal relation
// sets neighbours to clusters, and sets orientation, either to random or averaged or neighbours
void setNeighbours(int x, int y, int z, VoxelGrid<clusterData>& vGrid, glm::vec3 randOrientation) {
	glm::vec3 neighbourOrientation;

	if (x - 1 >= 0 && x - 1 <= vGrid.getDimensions().x) {
		vGrid.at(x - 1, y, z).isVoid = false; //sets the cell as a cluster

		// gets the neighbours of that cell, returns averaged neighbour orientation
		neighbourOrientation = checkNeighbours(x - 1, y, z, vGrid);
		// if the returned vector is 0, use the random orientation of the original cell.
		if (neighbourOrientation.x <= 0.0 && neighbourOrientation.y <= 0.0 && neighbourOrientation.z <= 0.0) {
			vGrid.at(x - 1, y, z).orientation = randOrientation;
		}
		// if the returned vector isn't 0, use the averaged neighbour vector
		else {
			vGrid.at(x - 1, y, z).orientation = neighbourOrientation;
		}
		
	}
	if (x + 1 >= 0 && x + 1 <= vGrid.getDimensions().x) {
		vGrid.at(x + 1, y, z).isVoid = false;

		neighbourOrientation = checkNeighbours(x + 1, y, z, vGrid);
		if (neighbourOrientation.x <= 0.0 && neighbourOrientation.y <= 0.0 && neighbourOrientation.z <= 0.0) {
			vGrid.at(x + 1, y, z).orientation = randOrientation;
		}
		else {
			vGrid.at(x + 1, y, z).orientation = randOrientation;
		}		
	}

	if (y - 1 >= 0 && y - 1 <= vGrid.getDimensions().y) {
		vGrid.at(x, y - 1, z).isVoid = false;

		neighbourOrientation = checkNeighbours(x, y - 1, z, vGrid);
		if (neighbourOrientation.x <= 0.0 && neighbourOrientation.y <= 0.0 && neighbourOrientation.z <= 0.0) {
			vGrid.at(x, y - 1, z).orientation = randOrientation;
		}
		else {
			vGrid.at(x, y - 1, z).orientation = randOrientation;
		}

	}
	if (y + 1 >= 0 && y + 1 <= vGrid.getDimensions().y) {
		vGrid.at(x, y + 1 , z).isVoid = false;

		neighbourOrientation = checkNeighbours(x, y + 1, z, vGrid);
		if (neighbourOrientation.x <= 0.0 && neighbourOrientation.y <= 0.0 && neighbourOrientation.z <= 0.0) {
			vGrid.at(x, y + 1, z).orientation = randOrientation;
		}
		else {
			vGrid.at(x, y + 1, z).orientation = randOrientation;
		}
	}

	if (z - 1 >= 0 && z - 1 <= vGrid.getDimensions().z) {
		vGrid.at(x, y, z - 1).isVoid = false;

		neighbourOrientation = checkNeighbours(x, y, z - 1, vGrid);
		if (neighbourOrientation.x <= 0.0 && neighbourOrientation.y <= 0.0 && neighbourOrientation.z <= 0.0) {
			vGrid.at(x, y, z - 1).orientation = randOrientation;
		}
		else {
			vGrid.at(x, y, z - 1).orientation = randOrientation;
		}
	}
	if (z + 1 >= 0 && z + 1 <= vGrid.getDimensions().z) {
		vGrid.at(x, y, z + 1).isVoid = false;

		neighbourOrientation = checkNeighbours(x, y, z + 1, vGrid);
		if (neighbourOrientation.x <= 0.0 && neighbourOrientation.y <= 0.0 && neighbourOrientation.z <= 0.0) {
			vGrid.at(x, y, z + 1).orientation = randOrientation;
		}
		else {
			vGrid.at(x, y, z + 1).orientation = randOrientation;
		}
	}
}

// Function to check neighbours to get their averaged orientation value if they have one
glm::vec3 checkNeighbours(int x, int y, int z, VoxelGrid<clusterData>& vGrid) {
	std::vector<glm::vec3> orientations;
	
	// Checks to see if the coordinate is within range
	if (x - 1 >= 0 && x - 1 <= vGrid.getDimensions().x) {
		// checks to see if the neighbour is a void - if not a void - get orientation
		if (vGrid.at(x - 1, y, z).isVoid == false) {
			orientations.push_back(vGrid.at(x - 1, y, z).orientation);
		}
		
	}
	if (x + 1 >= 0 && x + 1 <= vGrid.getDimensions().x) {
		if (vGrid.at(x + 1, y, z).isVoid == false) {
			orientations.push_back(vGrid.at(x + 1, y, z).orientation);
		}
	}

	if (y - 1 >= 0 && y - 1 <= vGrid.getDimensions().y) {
		if (vGrid.at(x, y - 1, z).isVoid == false) {
			orientations.push_back(vGrid.at(x, y - 1, z).orientation);
		}
	}

	if (y + 1 >= 0 && y + 1 <= vGrid.getDimensions().y) {
		if (vGrid.at(x, y + 1, z).isVoid == false) {
			orientations.push_back(vGrid.at(x, y + 1, z).orientation);
		}
	}

	if (z - 1 >= 0 && z - 1 <= vGrid.getDimensions().z) {
		if (vGrid.at(x, y, z - 1).isVoid == false) {
			orientations.push_back(vGrid.at(x, y, z - 1).orientation);
		}		
	}

	if (z + 1 >= 0 && z + 1 <= vGrid.getDimensions().z) {
		if (vGrid.at(x, y, z + 1).isVoid == false) {
			orientations.push_back(vGrid.at(x, y, z + 1).orientation);
		}		
	}
	
	// If there are some orientations in the list, average them and normalize them
	glm::vec3 averagedOrientation = glm::vec3(0.0,0.0,0.0);
	if (orientations.size() > 0) {
		for (int i = 0; i < orientations.size(); i++) {
			averagedOrientation += orientations.at(i);
		}
		normalize(averagedOrientation / (float) orientations.size());
	}

	return averagedOrientation;
}


