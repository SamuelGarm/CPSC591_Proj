#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/compatibility.hpp> // lerp
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/random.hpp"

#include "ShaderProgram.h"
#include <vector>
#include "cube.h"
#include "VoxelGrid.h"
#include "ClusterVoid.h"

namespace Graphics {
	struct RenderData {
		glm::mat4 transform = glm::mat4(1);
		glm::vec3 direction = glm::vec3(0);
	};

	std::vector<RenderData> instancedRenderData;
	ShaderProgram* orientationShader;
	ShaderProgram* gratingMaximaShader;
	GLuint voxels_vertexArray; //stores the state of how to render and interpert that buffer data
	GLuint voxels_vertexBuffer; //stores the vertex data in a buffer on the GPU
	GLuint voxels_instanceTransformBuffer; //stores data that is used for each instance of voxels

	void setupOpenGL() {
		//Rendering variables
		orientationShader = new ShaderProgram("shaders/voxel.vert", "shaders/voxel.frag");
		gratingMaximaShader = new ShaderProgram("shaders/grating.vert", "shaders/grating.frag");
		/*
		* Setup openGL structures for rendering voxel terrain
		* There are 2 buffers, one for the vertex geometry and the other storing data such as position that is applied to each voxel instance
		*/
		glGenVertexArrays(1, &voxels_vertexArray);
		glBindVertexArray(voxels_vertexArray);
		glGenBuffers(1, &voxels_vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, voxels_vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_data), cube_data, GL_STATIC_DRAW);

		//set up vertex data for the cube
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));

		//set up instancing buffers buffers
		glGenBuffers(1, &voxels_instanceTransformBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, voxels_instanceTransformBuffer);
		//set up instanced transformation matricies (I hate that I need to pass a 4x4 matrix...)
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4) + sizeof(glm::vec3), (void*)(0 * sizeof(glm::vec4)));
		glVertexAttribDivisor(2, 1);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4) + sizeof(glm::vec3), (void*)(1 * sizeof(glm::vec4)));
		glVertexAttribDivisor(3, 1);

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4) + sizeof(glm::vec3), (void*)(2 * sizeof(glm::vec4)));
		glVertexAttribDivisor(4, 1);

		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4) + sizeof(glm::vec3), (void*)(3 * sizeof(glm::vec4)));
		glVertexAttribDivisor(5, 1);

		//the direction
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4) + sizeof(glm::vec3), (void*)(4 * sizeof(glm::vec4)));
		glVertexAttribDivisor(6, 1);
	}

	struct clippingPlanes {
		float xClipMin = 0;
		float xClipMax = 0;
		float yClipMin = 0;
		float yClipMax = 0;
		float zClipMin = 0;
		float zClipMax = 0;
	};

	void loadVoxelgrid(VoxelGrid<clusterData>& grid, clippingPlanes* clip = nullptr) {
		instancedRenderData.clear();
		glm::vec3 upperBounds;
		glm::vec3 lowerBounds;
		if (clip == nullptr) {
			upperBounds = grid.getDimensions();
			lowerBounds = glm::vec3(0);
		}
		else {
			upperBounds = glm::vec3(clip->xClipMax, clip->yClipMax, clip->zClipMax);
			lowerBounds = glm::vec3(clip->xClipMin, clip->yClipMin, clip->zClipMin);
		}

		for (int x = lowerBounds.x; x < upperBounds.x; x++) {
			for (int y = lowerBounds.y; y < upperBounds.y; y++) {
				for (int z = lowerBounds.z; z < upperBounds.z; z++) {
					if (grid.at(x, y, z).material == Empty || grid.at(x, y, z).material == Void)
						continue;

					bool render = false;
					// Checks to see if the coordinate is within range
					if (x - 1 >= lowerBounds.x && x - 1 < upperBounds.x) {
						// checks to see if the neighbour is a void - if not a void - get orientation
						if (grid.at(x - 1, y, z).material == Void) {
							render = true;
						}
					} else { render = true; }

					if (x + 1 >= lowerBounds.x && x + 1 < upperBounds.x) {
						if (grid.at(x + 1, y, z).material == Void) {
							render = true;
						}
					} 
					else { render = true; }

					if (y - 1 >= lowerBounds.y && y - 1 < upperBounds.y) {
						if (grid.at(x, y - 1, z).material == Void) {
							render = true;
						}
					}
					else { render = true; }

					if (y + 1 >= lowerBounds.y && y + 1 < upperBounds.y) {
						if (grid.at(x, y + 1, z).material == Void) {
							render = true;
						}
					}
					else { render = true; }

					if (z - 1 >= lowerBounds.z && z - 1 < upperBounds.z) {
						if (grid.at(x, y, z - 1).material == Void) {
							render = true;
						}
					}
					else { render = true; }

					if (z + 1 >= lowerBounds.z && z + 1 < upperBounds.z) {
						if (grid.at(x, y, z + 1).material == Void) {
							render = true;
						}
					}
					else { render = true; }

					if (render) {
						RenderData data;
						data.transform = glm::translate(glm::mat4(1), glm::vec3(x, y, z));
						data.direction = grid.at(x, y, z).orientation;
						instancedRenderData.push_back(RenderData(data));
					}

				}
			}
		} //end of for loops
		std::cout << "Loaded " << instancedRenderData.size() << " voxels for rendering\n";
		std::cout << " Total voxel grid size is " << grid.getDimensions().x * grid.getDimensions().y * grid.getDimensions().z << "\n";

		glBindVertexArray(voxels_vertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, voxels_instanceTransformBuffer);
		glBufferData(GL_ARRAY_BUFFER, (sizeof(glm::mat4) + sizeof(glm::vec3)) * instancedRenderData.size(), instancedRenderData.data(), GL_STATIC_DRAW);


	}
}