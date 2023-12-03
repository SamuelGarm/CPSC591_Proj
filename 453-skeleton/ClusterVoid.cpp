#include "ClusterVoid.h"

float curNumOfClusters = 0.0f; // used for void ratio calculations

// Grid initialization
VoxelGrid<clusterData> setupGrid(int x, int y, int z) {
	VoxelGrid<clusterData> vGrid(x, y, z);     //set up the entire cluster structure as x,y,z dimension
	return vGrid;
}

// Function to setup void ratio
void setVoidRatio(VoxelGrid<clusterData>& vGrid, float ratio) {
	vGrid.setVoidRatio(ratio);
}

// Returns the set void ratio
float getVoidRatio(VoxelGrid<clusterData>& vGrid) {
	return vGrid.getVoidRatio();
}

// Returns the actual material ratios by iterating through all the voxels
glm::vec3 getClusterRatios(VoxelGrid<clusterData>& vGrid) {
	int clusterCount = 0;
	int voidCount = 0;
	int emptyCount = 0;

	for (int i = 0; i < vGrid.getDimensions().x; i++) {
		for (int j = 0; j < vGrid.getDimensions().y; j++) {
			for (int k = 0; k < vGrid.getDimensions().z; k++) {
				if (vGrid.at(i, j, k).material == Cluster) {
					clusterCount++;
				}
				else if (vGrid.at(i, j, k).material == Void) {
					voidCount++;
				}
				else if (vGrid.at(i, j, k).material == Empty) {
					emptyCount++;
				}
			}
		}
	}
	return glm::vec3(clusterCount, voidCount, emptyCount);
}

// Creates a randomized direction vector, and normalizes it
glm::vec3 randOrientation() {
	float randOrientation_x = glm::linearRand<float>(0, 360);
	float randOrientation_y = glm::linearRand<float>(0, 360);
	float randOrientation_z = glm::linearRand<float>(0, 360);
	//return normalize(glm::vec3(randOrientation_x, randOrientation_y, randOrientation_z));
	return glm::vec3(randOrientation_x, randOrientation_y, randOrientation_z);
}

// The main algorithm to distribute clusters and orient them in the structure based on neighbouring heuristics
void distributeVoidClusters(VoxelGrid<clusterData>& vGrid) {
	// Step 1 - initializes every cluster as a void
	for (int i = 0; i < vGrid.getDimensions().x; i++) {
		for (int j = 0; j < vGrid.getDimensions().y; j++) {
			for (int k = 0; k < vGrid.getDimensions().z; k++) {
				vGrid.at(i, j, k).material = Void;

			}
		}
	}
	
	float totalNumberOfCells = vGrid.getDimensions().x * vGrid.getDimensions().y * vGrid.getDimensions().z;
	//std::cout << "total number of cells: " << totalNumberOfCells << std::endl;

	// While the ratio of clusters to total cells is less than the void ratio, keep iterating
	while (curNumOfClusters / totalNumberOfCells < vGrid.getVoidRatio()) {
		// selects cells at random 
		int current_x = glm::linearRand<int>(0, vGrid.getDimensions().x-1);
		int current_y = glm::linearRand<int>(0, vGrid.getDimensions().y-1);
		int current_z = glm::linearRand<int>(0, vGrid.getDimensions().z-1);
		clusterData& currCluster = vGrid.at(current_x, current_y, current_z);
		// sets it as a cluster if it's a void
		if (currCluster.material == Void) {
			currCluster.material = Cluster;
			curNumOfClusters++;
		}

		// Checks if neighbours orientations, if the average is 0.0 use the random rotation
		// if neighbours have orientations, uses the average instead
		glm::vec3 neighbourOrientation = checkNeighbours(current_x, current_y, current_z, vGrid);
		if (neighbourOrientation.x <= 0.0 && neighbourOrientation.y <= 0.0 && neighbourOrientation.z <= 0.0) {
			vGrid.at(current_x, current_y, current_z).orientation = randOrientation();
		}
		else {
			vGrid.at(current_x, current_y, current_z).orientation = neighbourOrientation;
		}

		setNeighbours(current_x, current_y, current_z, vGrid);
	}
}

// 6 neighbours - orthogonal relation
// sets neighbours to clusters, and sets orientation, either to random or averaged or neighbours
void setNeighbours(int x, int y, int z, VoxelGrid<clusterData>& vGrid) {
	glm::vec3 neighbourOrientation;

	if (x - 1 >= 0 && x - 1 < vGrid.getDimensions().x) {
		if (vGrid.at(x - 1, y, z).material == Void) {
			vGrid.at(x - 1, y, z).material = Cluster; //sets the cell as a cluster
			curNumOfClusters++;

			// gets the neighbours of that cell, returns averaged neighbour orientation
			neighbourOrientation = checkNeighbours(x - 1, y, z, vGrid);
			// if the returned vector is 0, use the random orientation of the original cell.
			if (neighbourOrientation.x <= 0.0 && neighbourOrientation.y <= 0.0 && neighbourOrientation.z <= 0.0) {
				vGrid.at(x - 1, y, z).orientation = randOrientation();
			}
			// if the returned vector isn't 0, use the averaged neighbour vector
			else {
				vGrid.at(x - 1, y, z).orientation = neighbourOrientation;
			}
		}				
	}

	if (x + 1 >= 0 && x + 1 < vGrid.getDimensions().x) {
		if (vGrid.at(x + 1, y, z).material == Void) {
			vGrid.at(x + 1, y, z).material = Cluster;
			curNumOfClusters++;

			neighbourOrientation = checkNeighbours(x + 1, y, z, vGrid);
			if (neighbourOrientation.x <= 0.0 && neighbourOrientation.y <= 0.0 && neighbourOrientation.z <= 0.0) {
				vGrid.at(x + 1, y, z).orientation = randOrientation();
			}
			else {
				vGrid.at(x + 1, y, z).orientation = neighbourOrientation;
			}
		}	
	}

	if (y - 1 >= 0 && y - 1 < vGrid.getDimensions().y) {
		if (vGrid.at(x, y - 1, z).material == Void) {
			vGrid.at(x, y - 1, z).material = Cluster;
			curNumOfClusters++;

			neighbourOrientation = checkNeighbours(x, y - 1, z, vGrid);
			if (neighbourOrientation.x <= 0.0 && neighbourOrientation.y <= 0.0 && neighbourOrientation.z <= 0.0) {
				vGrid.at(x, y - 1, z).orientation = randOrientation();
			}
			else {
				vGrid.at(x, y - 1, z).orientation = neighbourOrientation;
			}
		}
	}

	if (y + 1 >= 0 && y + 1 < vGrid.getDimensions().y) {
		if (vGrid.at(x, y + 1, z).material == Void) {
			vGrid.at(x, y + 1, z).material = Cluster;
			curNumOfClusters++;

			neighbourOrientation = checkNeighbours(x, y + 1, z, vGrid);
			if (neighbourOrientation.x <= 0.0 && neighbourOrientation.y <= 0.0 && neighbourOrientation.z <= 0.0) {
				vGrid.at(x, y + 1, z).orientation = randOrientation();
			}
			else {
				vGrid.at(x, y + 1, z).orientation = neighbourOrientation;
			}
		}
	}

	if (z - 1 >= 0 && z - 1 < vGrid.getDimensions().z) {
		if (vGrid.at(x, y, z - 1).material == Void) {
			vGrid.at(x, y, z - 1).material = Cluster;
			curNumOfClusters++;

			neighbourOrientation = checkNeighbours(x, y, z - 1, vGrid);
			if (neighbourOrientation.x <= 0.0 && neighbourOrientation.y <= 0.0 && neighbourOrientation.z <= 0.0) {
				vGrid.at(x, y, z - 1).orientation = randOrientation();
			}
			else {
				vGrid.at(x, y, z - 1).orientation = neighbourOrientation;
			}
		}
	}

	if (z + 1 >= 0 && z + 1 < vGrid.getDimensions().z) {
		if (vGrid.at(x, y, z + 1).material == Void) {
			vGrid.at(x, y, z + 1).material = Cluster;
			curNumOfClusters++;

			neighbourOrientation = checkNeighbours(x, y, z + 1, vGrid);
			if (neighbourOrientation.x <= 0.0 && neighbourOrientation.y <= 0.0 && neighbourOrientation.z <= 0.0) {
				vGrid.at(x, y, z + 1).orientation = randOrientation();
			}
			else {
				vGrid.at(x, y, z + 1).orientation = neighbourOrientation;
			}
		}
	}
}

// Function to check neighbours to get their averaged orientation value if they have one
glm::vec3 checkNeighbours(int x, int y, int z, VoxelGrid<clusterData>& vGrid) {
	std::vector<glm::vec3> orientations;
	
	// Checks to see if the coordinate is within range
	if (x - 1 >= 0 && x - 1 < vGrid.getDimensions().x) {
		// checks to see if the neighbour is a void - if not a void - get orientation
		if (vGrid.at(x - 1, y, z).material == Cluster) {
			orientations.push_back(vGrid.at(x - 1, y, z).orientation);
		}
		
	}
	if (x + 1 >= 0 && x + 1 < vGrid.getDimensions().x) {
		if (vGrid.at(x + 1, y, z).material == Cluster) {
			orientations.push_back(vGrid.at(x + 1, y, z).orientation);
		}
	}

	if (y - 1 >= 0 && y - 1 < vGrid.getDimensions().y) {
		if (vGrid.at(x, y - 1, z).material == Cluster) {
			orientations.push_back(vGrid.at(x, y - 1, z).orientation);
		}
	}

	if (y + 1 >= 0 && y + 1 < vGrid.getDimensions().y) {
		if (vGrid.at(x, y + 1, z).material == Cluster) {
			orientations.push_back(vGrid.at(x, y + 1, z).orientation);
		}
	}

	if (z - 1 >= 0 && z - 1 < vGrid.getDimensions().z) {
		if (vGrid.at(x, y, z - 1).material == Cluster) {
			orientations.push_back(vGrid.at(x, y, z - 1).orientation);
		}		
	}

	if (z + 1 >= 0 && z + 1 < vGrid.getDimensions().z) {
		if (vGrid.at(x, y, z + 1).material == Cluster) {
			orientations.push_back(vGrid.at(x, y, z + 1).orientation);
		}		
	}
	
	// If there are some orientations in the list, average them and normalize them
	glm::vec3 averagedOrientation = glm::vec3(0.0,0.0,0.0);
	if (orientations.size() > 0) {
		for (int i = 0; i < orientations.size(); i++) {
			averagedOrientation += orientations.at(i);
		}
		//averagedOrientation = normalize(averagedOrientation / (float) orientations.size());
		averagedOrientation = averagedOrientation / (float)orientations.size();
	}



	return averagedOrientation;
}


