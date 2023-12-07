#include "ClusterVoid.h"
#include "panel.h"
#include <random>
#include <set>
#include <algorithm>

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

// From chat GPT on how to get an exponential distribution
// the mean input is the average ?
float exponential_random(float mean) {
	std::random_device rd;
	std::default_random_engine generator(rd());
	std::exponential_distribution<float> distribution(1.f / mean);
	return distribution(generator);
}

// sets every voxel in the grid to a void, and resets the orientation
void setAllVoid(VoxelGrid<clusterData>& vGrid) {
	for (int i = 0; i < vGrid.getDimensions().x; i++) {
		for (int j = 0; j < vGrid.getDimensions().y; j++) {
			for (int k = 0; k < vGrid.getDimensions().z; k++) {
				vGrid.at(i, j, k).material = Void;
				vGrid.at(i, j, k).orientation = glm::vec3(0);

			}
		}
	}
	curNumOfClusters = 0;
}

// The main algorithm to distribute clusters and orient them in the structure based on neighbouring heuristics
void distributeVoidClusters(VoxelGrid<clusterData>& vGrid, bool sampleNeighbours) {
	setAllVoid(vGrid);

	float totalNumberOfCells = vGrid.getDimensions().x * vGrid.getDimensions().y * vGrid.getDimensions().z;
	//std::cout << "total number of cells: " << totalNumberOfCells << std::endl;
	srand(time(0));

	// While the ratio of clusters to total cells is less than the void ratio, keep iterating
	while (curNumOfClusters / totalNumberOfCells < vGrid.getVoidRatio()) {
		// selects cells at random 
		int current_x = glm::linearRand<int>(0, vGrid.getDimensions().x-1);
		int current_y = glm::linearRand<int>(0, vGrid.getDimensions().y-1);
		int current_z = glm::linearRand<int>(0, vGrid.getDimensions().z-1);
		//std::cout << current_x << "," << current_y << "," << current_z << std::endl;
		clusterData& currCluster = vGrid.at(current_x, current_y, current_z);
		// sets it as a cluster if it's a void
		if (currCluster.material == Void) {
			currCluster.material = Cluster;
			curNumOfClusters++;
		}

		// Checks if neighbours orientations, if the average is 0.0 use the random rotation
		// if neighbours have orientations, uses the average instead
		glm::vec3 neighbourOrientation = glm::vec3(0);
		if (sampleNeighbours) {
			neighbourOrientation = checkNeighbours(current_x, current_y, current_z, vGrid);
		}
		
		if (neighbourOrientation.x <= 0.0 && neighbourOrientation.y <= 0.0 && neighbourOrientation.z <= 0.0) {
			vGrid.at(current_x, current_y, current_z).orientation = randOrientation();
		}
		else {
			vGrid.at(current_x, current_y, current_z).orientation = neighbourOrientation;
		}

		if (sampleNeighbours) setNeighbours(current_x, current_y, current_z, vGrid);
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
	
	// Averages the orientations
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

// Distributes clusters in spheroids with radius based on an exponential distribution
// function derived from a mean radius
void distributeVoidClusterV2(VoxelGrid<clusterData>& vGrid, float meanRadius, bool sampleNeighbours) {
	setAllVoid(vGrid);

	float totalNumberOfCells = vGrid.getDimensions().x * vGrid.getDimensions().y * vGrid.getDimensions().z;
	//std::cout << "total number of cells: " << totalNumberOfCells << std::endl;
	srand(time(0));

	// While the ratio of clusters to total cells is less than the void ratio, keep iterating
	while (curNumOfClusters / totalNumberOfCells < vGrid.getVoidRatio()) {
		//system("cls");
		//std::cout << "Current Cluster %: " << 100.f*(curNumOfClusters / totalNumberOfCells)
		//	<< ", Target %: " << 100.f*(vGrid.getVoidRatio()) << "|| % Complete: "
		//	<< 100.f * ((curNumOfClusters / totalNumberOfCells) / vGrid.getVoidRatio())
		//	<< std::endl;

		// selects cells at random 
		int current_x = glm::linearRand<int>(0, vGrid.getDimensions().x - 1);
		int current_y = glm::linearRand<int>(0, vGrid.getDimensions().y - 1);
		int current_z = glm::linearRand<int>(0, vGrid.getDimensions().z - 1);
		//std::cout << current_x << "," << current_y << "," << current_z << std::endl;

		// Generates a random radius based on a mean value exponential distribution
		float randRadius = exponential_random(meanRadius);

		// Setting rotation values
		glm::vec3 rOrientation = glm::vec3(0);
		// if neighbours are sampled, check the neighbours in a radius and collect their average facing
		if (sampleNeighbours) {
			bool fixedRadius = panel::fixedRadius;
			rOrientation = checkNeighboursRadius(current_x, current_y, current_z, vGrid, 
				(randRadius * 2.f), fixedRadius);
		}
		// if the orientation vector is 0 (no neighbours, or not sampled) give it a random orientation
		if (rOrientation.x == 0 && rOrientation.y == 0 && rOrientation.z == 0) {
			rOrientation = randOrientation();
		}
		
		// Get a list of surface points on a paramaterized sphere
		std::vector<glm::vec3> fillList = sphereParameterization(randRadius);

		// Iterate through the surface points list
		for (int i = 0; i < fillList.size(); i++) {
			float x = fillList.at(i).x + current_x;
			float y = fillList.at(i).y + current_y;
			float z = fillList.at(i).z + current_z;

			// if the current sample point is less than the radius
			if (x < current_x) {
				// fill all voxels on that row to the middle
				for (int j = x; j < current_x; j++) {
					// if the voxel being filled is within the range
					if (j >= 0 && j < vGrid.getDimensions().x &&
						y >= 0 && y < vGrid.getDimensions().y &&
						z >= 0 && z < vGrid.getDimensions().z) {
							if (vGrid.at(j, y, z).material != Cluster) {
								vGrid.at(j, y, z).material = Cluster;
								vGrid.at(j, y, z).orientation = rOrientation;
								curNumOfClusters++;
							}
							
					}
				}
			}
			// if the current sampel point is greater than the radius
			else if (x >= current_x) {
				// fill all voxels on that row to the middle
				for (int j = x; j >= current_x; j--) {
					// if the voxel being filled is within the range
					if (j >= 0 && j < vGrid.getDimensions().x &&
						y >= 0 && y < vGrid.getDimensions().y &&
						z >= 0 && z < vGrid.getDimensions().z) {
							if (vGrid.at(j, y, z).material != Cluster) {
								vGrid.at(j, y, z).material = Cluster;
								vGrid.at(j, y, z).orientation = rOrientation;
								curNumOfClusters++;
							}
					}
				}
			}
		} 
	}
}

// Checks the neighbours in a radius, collects each voxel's orientation and averages total
glm::vec3 checkNeighboursRadius(int in_x, int in_y, int in_z, VoxelGrid<clusterData>& vGrid, 
	float in_radius, bool fixedRadius) {
	
	float radius = 0.f;

	// if fixed radius uses the dimensions of the input grid as a fixed sampling radius
	if (fixedRadius) {
		// Pick the smallest dimension of the grid to set the radius of search
		radius = vGrid.getDimensions().x;
		if (radius > vGrid.getDimensions().y) radius = vGrid.getDimensions().y;
		if (radius > vGrid.getDimensions().z) radius = vGrid.getDimensions().z;
		radius = radius / 2.f;
	}
	// if fixed radius is not set, uses in the randomized input radius of the spheroid
	else {
		radius = in_radius;
	}


	// Get a list of surface points on a paramaterized sphere
	std::vector<glm::vec3> fillList = sphereParameterization(radius);

	std::vector<glm::vec3> orientations;;

	// Iterate through the surface points list
	for (int i = 0; i < fillList.size(); i++) {
		float x = fillList.at(i).x + in_x;
		float y = fillList.at(i).y + in_y;
		float z = fillList.at(i).z + in_z;

		// if the current sample point is less than the radius
		if (x < in_x) {
			// fill all voxels on that row to the middle
			for (int j = x; j < in_x; j++) {
				// if the voxel being filled is within the range
				if (j >= 0 && j < vGrid.getDimensions().x &&
					y >= 0 && y < vGrid.getDimensions().y &&
					z >= 0 && z < vGrid.getDimensions().z) {
					if (vGrid.at(j, y, z).material == Cluster) {
						orientations.push_back(vGrid.at(j, y, z).orientation);
					}

				}
			}
		}
		// if the current sampel point is greater than the radius
		else if (x >= in_x) {
			// fill all voxels on that row to the middle
			for (int j = x; j >= in_x; j--) {
				// if the voxel being filled is within the range
				if (j >= 0 && j < vGrid.getDimensions().x &&
					y >= 0 && y < vGrid.getDimensions().y &&
					z >= 0 && z < vGrid.getDimensions().z) {
					if (vGrid.at(j, y, z).material == Cluster) {
						orientations.push_back(vGrid.at(j, y, z).orientation);
					}
				}
			}
		}
	}
	// Sorts the collected orientations array and removes all
	// duplicated entries to give better average
	// duplicated entries will probabilistically come from the same neighbour
	//std::sort(orientations.begin(), orientations.end());
	orientations.erase(std::unique(orientations.begin(), orientations.end()), orientations.end());

	// Averages the orientations
	glm::vec3 averagedOrientation = glm::vec3(0.0, 0.0, 0.0);
	//std::cout << "sampled neighbour orientations count: " << orientations.size() << std::endl;
	if (orientations.size() > 0) {
		for (int i = 0; i < orientations.size(); i++) {
			averagedOrientation += orientations.at(i);
		}
		//averagedOrientation = normalize(averagedOrientation / (float) orientations.size());
		averagedOrientation = averagedOrientation / (float)orientations.size();
	}
	return averagedOrientation;
}

// Input a, b, c are the xyz dimensions of a 'rectangle'
// uses these dimensions to find the 'radius' of each axis (divide by 2)
// then uses parametric ellipsoid equation to iterate through the u and v 
// directions on a parametric surface
// https://mathworld.wolfram.com/Ellipsoid.html
std::vector<glm::vec3> ellipsoid(float a, float b, float c) {
	int uLength = 500;
	int vLength = 500;
	float x, y, z;

	//float lowestX = 0;
	//float highestX = 0;
	//float lowestY = 0;
	//float highestY = 0;
	//float lowestZ = 0;
	//float highestZ = 0;

	std::vector<glm::vec3> coords;

	// This parametric surface is working as if the origin is at (0,0,0)
	// you will need to add (a/2.f) to x to shift of the origin over by the radius
	// need to do this for every axis to allign it to the voxel grid
	for (int u = 0; u < uLength; u++) {
		for (int v = 0; v < vLength; v++) {
			x = (a / 2.f) * cos(u) * sin(v);
			y = (b / 2.f) * sin(u) * sin(v);
			z = (c / 2.f) * cos(v);

			//if (x > highestX) highestX = x;
			//if (x < lowestX) lowestX = x;
			//if (y > highestY) highestY = y;
			//if (y < lowestY) lowestY = y;
			//if (z > highestZ) highestZ = z;
			//if (z < lowestZ) lowestZ = z;

			//printf("parametric ellipsoid: %.f, %.f, %.f \n", x, y, z);

			// Calls a post-processing step to clamp values to the voxel rectangle range
			coords.push_back(postProcessVec(glm::vec3(x,y,z),a,b,c));
		}
	}
	//std::cout << "highest x: " << highestX << std::endl;
	//std::cout << "lowest x: " << lowestX << std::endl;
	//std::cout << "highest y: " << highestY << std::endl;
	//std::cout << "lowest y: " << lowestY << std::endl;
	//std::cout << "highest z: " << highestZ << std::endl;
	//std::cout << "lowest z: " << lowestZ << std::endl;

	return coords;
}

//https://www.wolframalpha.com/input/?i=parametrization+of+a+sphere
std::vector<glm::vec3> sphereParameterization(float radius) {
	int uLength = radius * 15;
	int vLength = radius * 15;
	float x, y, z;

	std::vector<glm::vec3> coords;

	for (int u = 0; u < uLength; u++) {
		for (int v = 0; v < vLength; v++) {
			x = radius * cos(u) * sin(v);
			y = radius * sin(u) * sin(v);
			z = radius * cos(v);

			coords.push_back(glm::vec3(x, y, z));
		}
	}
	return coords;
}

// Post process function to clamp given values to the voxel elipsoid
// input parameters are voxel rectangle dimensions
glm::vec3 postProcessVec(glm::vec3 vec, float a, float b, float c) {
	float x = vec.x;
	float y = vec.y;
	float z = vec.z;

	// Offsets to put the parametric ellipsoid center
	// in the centre of the voxel rectangle
	// -1 to keep it in the index range of voxels
	x = x + (a / 2.f) - 1;
	y = y + (b / 2.f) - 1;
	z = z + (c / 2.f) - 1;

	// Sometimes values below zero are returned
	// these values are clamped to 0
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (z < 0) z = 0;

	// Sometimes values are beyond the max range
	// if this happens clamp to max range
	if (x > a) x = a-1;
	if (y > b) y = b-1;
	if (z > c) z = c-1;

	return glm::vec3(x, y, z);
}

// Trims the rectangle vGrid into an ellipsoid 
void trimVGrid(VoxelGrid<clusterData>& vGrid) {
	auto trimList = ellipsoid(vGrid.getDimensions().x, vGrid.getDimensions().y, vGrid.getDimensions().z);

	// Iterate through the trim list one z plane at a time
	for (int i = 0; i < trimList.size(); i++) {

		int z = ceil(trimList.at(i).z);
		if (z > vGrid.getDimensions().z) z = vGrid.getDimensions().z - 1;

		// If the entries for x and y in the trim list are below the halfway point
		// remove all voxels on the x axis from 0 to the trim point
		// remove all voxels on the y axis from 0 to the trim point
		if (trimList.at(i).x < ((float)vGrid.getDimensions().x / 2.0) &&
			trimList.at(i).y < ((float)vGrid.getDimensions().y / 2.0)) {

			for (int x = 0; x < trimList.at(i).x; x++) {
				for (int y = 0; y < trimList.at(i).y; y++) {
					vGrid.at(x, y, z).material = Empty;
				}
			}
		}

		// If the entry for x is below the halfway point, and the entry for y is above the halfway point
		// remove all voxels on the x axis from 0 to the trim point
		// remove all voxels on the y axis from the trim point to max
		if (trimList.at(i).x < ((float)vGrid.getDimensions().x / 2.0) &&
			trimList.at(i).y >= ((float)vGrid.getDimensions().y / 2.0)) {

			for (int x = 0; x < trimList.at(i).x; x++) {
				for (int y = trimList.at(i).y; y < vGrid.getDimensions().y; y++) {
					vGrid.at(x, y, z).material = Empty;
				}
			}
		}

		// If the entry for x is above the halfway point, and entry for y is below the halfway point
		// remove all voxels on the x axis from the trim point to max
		// remove all voxels on the y axis from 0 to the trim point
		if (trimList.at(i).x >= ((float)vGrid.getDimensions().x / 2.0) &&
			trimList.at(i).y < ((float)vGrid.getDimensions().y / 2.0)) {

			for (int x = trimList.at(i).x; x < vGrid.getDimensions().x; x++) {
				for (int y = 0; y < trimList.at(i).y; y++) {
					vGrid.at(x, y, z).material = Empty;
				}
			}
		}

		// If the entries for x and y are above the halfway point
		// remove all voxels on the x axis from the trim point to max
		// remove all voxels on the y axis from the trim point to max
		if (trimList.at(i).x >= ((float)vGrid.getDimensions().x / 2.0) &&
			trimList.at(i).y >= ((float)vGrid.getDimensions().y / 2.0)) {

			for (int x = trimList.at(i).x; x < vGrid.getDimensions().x; x++) {
				for (int y = trimList.at(i).y; y < vGrid.getDimensions().y; y++) {
					vGrid.at(x, y, z).material = Empty;
				}
			}
		}

	}
}
