#include "ClusterVoid.h"

float curNumOfClusters = 0.0f; // used for void ratio calculations
float voidRatio = 0.75f;        // void ratio used as a clamp for structural setup

// Grid initialization
VoxelGrid<clusterData> setupGrid(int x, int y, int z) {
	VoxelGrid<clusterData> vGrid(x, y, z);     //set up the entire cluster structure as x,y,z dimension
	return vGrid;
}

// Function to setup void ratio
void setVoidRatio(float ratio) {
	voidRatio = ratio;
}

// The main algorithm to distribute clusters and orient them in the structure based on neighbouring heuristics
void distributeVoidClusters(VoxelGrid<clusterData>& vGrid) {
	// Step 1 - initializes every cluster as a void
	for (int i = 0; i < vGrid.getDimensions().x; i++) {
		for (int j = 0; j < vGrid.getDimensions().y; j++) {
			for (int k = 0; k < vGrid.getDimensions().z; k++) {
				vGrid.at(i, j, k).isVoid = true;
			}
		}
	}
	
	float totalNumberOfCells = vGrid.getDimensions().x * vGrid.getDimensions().y * vGrid.getDimensions().z;
	std::cout << "total number of cells: " << totalNumberOfCells << std::endl;

	// While the ratio of clusters to total cells is less than the void ratio, keep iterating
	while (curNumOfClusters / totalNumberOfCells < voidRatio) {
		// selects cells at random 
		int current_x = glm::linearRand<int>(0, vGrid.getDimensions().x-1);
		int current_y = glm::linearRand<int>(0, vGrid.getDimensions().y-1);
		int current_z = glm::linearRand<int>(0, vGrid.getDimensions().z-1);
		clusterData& currCluster = vGrid.at(current_x, current_y, current_z);
		// sets it as a cluster if it's a void
		if (currCluster.isVoid == true) {
			currCluster.isVoid = false;
			curNumOfClusters++;
		}

		// sets the rotation randomly
		float randOrientation_x = glm::linearRand<float>(0, 360);
		float randOrientation_y = glm::linearRand<float>(0, 360);
		float randOrientation_z = glm::linearRand<float>(0, 360);
		glm::vec3 randOrientation = glm::vec3(randOrientation_x, randOrientation_y, randOrientation_z);
		currCluster.orientation = randOrientation;

		// Checks if neighbours orientations, if the average is 0.0 use the random rotation
		// if neighbours have orientations, uses the average instead
		glm::vec3 neighbourOrientation = checkNeighbours(current_x, current_y, current_z, vGrid);
		if (neighbourOrientation.x <= 0.0 && neighbourOrientation.y <= 0.0 && neighbourOrientation.z <= 0.0) {
			vGrid.at(current_x, current_y, current_z).orientation = randOrientation;
		}
		else {
			vGrid.at(current_x, current_y, current_z).orientation = neighbourOrientation;
		}

		setNeighbours(current_x, current_y, current_z, vGrid, randOrientation);
	}
	//std::cout << "Ratio: " << curNumOfClusters / totalNumberOfCells << std::endl;
	//std::cout << "Number of Clusters: " << curNumOfClusters << std::endl;
}

// 6 neighbours - orthogonal relation
// sets neighbours to clusters, and sets orientation, either to random or averaged or neighbours
void setNeighbours(int x, int y, int z, VoxelGrid<clusterData>& vGrid, glm::vec3 randOrientation) {
	glm::vec3 neighbourOrientation;

	if (x - 1 >= 0 && x - 1 < vGrid.getDimensions().x) {
		if (vGrid.at(x - 1, y, z).isVoid == true) {
			vGrid.at(x - 1, y, z).isVoid = false; //sets the cell as a cluster
			curNumOfClusters++;
		}

		// gets the neighbours of that cell, returns averaged neighbour orientation
		neighbourOrientation = checkNeighbours(x - 1, y, z, vGrid);
		// if the returned vector is 0, use the random orientation of the original cell.
		if (neighbourOrientation.x <= 0.0 && neighbourOrientation.y < 0.0 && neighbourOrientation.z <= 0.0) {
			vGrid.at(x - 1, y, z).orientation = randOrientation;
		}
		// if the returned vector isn't 0, use the averaged neighbour vector
		else {
			vGrid.at(x - 1, y, z).orientation = neighbourOrientation;
		}
		
	}
	if (x + 1 >= 0 && x + 1 < vGrid.getDimensions().x) {
		if (vGrid.at(x + 1, y, z).isVoid == true) {
			vGrid.at(x + 1, y, z).isVoid = false;
			curNumOfClusters++;
		}

		neighbourOrientation = checkNeighbours(x + 1, y, z, vGrid);
		if (neighbourOrientation.x <= 0.0 && neighbourOrientation.y < 0.0 && neighbourOrientation.z <= 0.0) {
			vGrid.at(x + 1, y, z).orientation = randOrientation;
		}
		else {
			vGrid.at(x + 1, y, z).orientation = neighbourOrientation;
		}		
	}

	if (y - 1 >= 0 && y - 1 < vGrid.getDimensions().y) {
		if (vGrid.at(x, y - 1, z).isVoid == true) {
			vGrid.at(x, y - 1, z).isVoid = false;
			curNumOfClusters++;
		}

		neighbourOrientation = checkNeighbours(x, y - 1, z, vGrid);
		if (neighbourOrientation.x <= 0.0 && neighbourOrientation.y < 0.0 && neighbourOrientation.z <= 0.0) {
			vGrid.at(x, y - 1, z).orientation = randOrientation;
		}
		else {
			vGrid.at(x, y - 1, z).orientation = neighbourOrientation;
		}

	}
	if (y + 1 >= 0 && y + 1 < vGrid.getDimensions().y) {
		if (vGrid.at(x, y + 1, z).isVoid == true) {
			vGrid.at(x, y + 1, z).isVoid = false;
			curNumOfClusters++;
		}

		neighbourOrientation = checkNeighbours(x, y + 1, z, vGrid);
		if (neighbourOrientation.x <= 0.0 && neighbourOrientation.y < 0.0 && neighbourOrientation.z <= 0.0) {
			vGrid.at(x, y + 1, z).orientation = randOrientation;
		}
		else {
			vGrid.at(x, y + 1, z).orientation = neighbourOrientation;
		}
	}

	if (z - 1 >= 0 && z - 1 < vGrid.getDimensions().z) {
		if (vGrid.at(x, y, z - 1).isVoid == true) {
			vGrid.at(x, y, z - 1).isVoid = false;
			curNumOfClusters++;
		}

		neighbourOrientation = checkNeighbours(x, y, z - 1, vGrid);
		if (neighbourOrientation.x <= 0.0 && neighbourOrientation.y < 0.0 && neighbourOrientation.z <= 0.0) {
			vGrid.at(x, y, z - 1).orientation = randOrientation;
		}
		else {
			vGrid.at(x, y, z - 1).orientation = neighbourOrientation;
		}
	}
	if (z + 1 >= 0 && z + 1 < vGrid.getDimensions().z) {
		if (vGrid.at(x, y, z + 1).isVoid == true) {
			vGrid.at(x, y, z + 1).isVoid = false;
			curNumOfClusters++;
		}

		neighbourOrientation = checkNeighbours(x, y, z + 1, vGrid);
		if (neighbourOrientation.x <= 0.0 && neighbourOrientation.y <= 0.0 && neighbourOrientation.z <= 0.0) {
			vGrid.at(x, y, z + 1).orientation = randOrientation;
		}
		else {
			vGrid.at(x, y, z + 1).orientation = neighbourOrientation;
		}
	}
}

// Function to check neighbours to get their averaged orientation value if they have one
glm::vec3 checkNeighbours(int x, int y, int z, VoxelGrid<clusterData>& vGrid) {
	std::vector<glm::vec3> orientations;
	
	// Checks to see if the coordinate is within range
	if (x - 1 >= 0 && x - 1 < vGrid.getDimensions().x) {
		// checks to see if the neighbour is a void - if not a void - get orientation
		if (vGrid.at(x - 1, y, z).isVoid == false) {
			orientations.push_back(vGrid.at(x - 1, y, z).orientation);
		}
		
	}
	if (x + 1 >= 0 && x + 1 < vGrid.getDimensions().x) {
		if (vGrid.at(x + 1, y, z).isVoid == false) {
			orientations.push_back(vGrid.at(x + 1, y, z).orientation);
		}
	}

	if (y - 1 >= 0 && y - 1 < vGrid.getDimensions().y) {
		if (vGrid.at(x, y - 1, z).isVoid == false) {
			orientations.push_back(vGrid.at(x, y - 1, z).orientation);
		}
	}

	if (y + 1 >= 0 && y + 1 < vGrid.getDimensions().y) {
		if (vGrid.at(x, y + 1, z).isVoid == false) {
			orientations.push_back(vGrid.at(x, y + 1, z).orientation);
		}
	}

	if (z - 1 >= 0 && z - 1 < vGrid.getDimensions().z) {
		if (vGrid.at(x, y, z - 1).isVoid == false) {
			orientations.push_back(vGrid.at(x, y, z - 1).orientation);
		}		
	}

	if (z + 1 >= 0 && z + 1 < vGrid.getDimensions().z) {
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
		averagedOrientation = normalize(averagedOrientation / (float) orientations.size());
		//std::cout << averagedOrientation.x << "," << averagedOrientation.y << "," << averagedOrientation.z << std::endl;
	}

	return averagedOrientation;
}


