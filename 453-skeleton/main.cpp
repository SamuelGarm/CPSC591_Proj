#define _USE_MATH_DEFINES
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <chrono>

#include <glm/gtx/vector_query.hpp>

#include "Geometry.h"
#include "GLDebug.h"
#include "Log.h"
#include "ShaderProgram.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"
#include "imagebuffer.h"
//#include "RayTrace.h"
//#include "Scene.h"
//#include "Lighting.h"
#include "panel.h"
#include "ClusterVoid.h"
#include "RayTraceVoxel.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "PhotonMapping.h"
#include "Camera.h"
#include "FreeCamOpenGLSetup.h"

#include "RayTraceVoxel.h"

// EXAMPLE CALLBACKS
class Assignment5 : public CallbackInterface {

public:
	Assignment5() {
		//viewPoint = glm::vec3(0, 0, 2);
		//scene = initScene1();
		//std::cout << "Rendering scene 1\n";
		//raytraceImage(scene, outputImage, viewPoint);
		//std::cout << "Finished\n";
	}

	virtual void mouseButtonCallback(int button, int action, int mods) {
		if (true) { //panel::renderPipeline != 2) {
			if (button == GLFW_MOUSE_BUTTON_LEFT) {
				if (action == GLFW_PRESS) {
					clickX = mouseX;
					clickY = mouseY;
					leftMouseButtonPressed = true;
				}
				else if (action == GLFW_RELEASE) {
					leftMouseButtonPressed = false;
					firstMouse = true;
				}
			}
		}
	}

	virtual void keyCallback(int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
			shouldQuit = true;
		}

		if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
			panel::renderPipeline = 1;
			panel::pipelineChanged = true;
		}

		if (true) {//panel::renderPipeline != 2) {
			if (action == GLFW_PRESS) {
				if (key == GLFW_KEY_P)
					panel::showPanel = !panel::showPanel;
				if (key == GLFW_KEY_W)
					camera.fspeed += 1.0f;
				if (key == GLFW_KEY_S)
					camera.fspeed += -1.0f;
				if (key == GLFW_KEY_A)
					camera.hspeed += -1.0f;
				if (key == GLFW_KEY_D)
					camera.hspeed += 1.0f;
			}
			else if (action == GLFW_RELEASE)
			{
				if (key == GLFW_KEY_W)
					camera.fspeed -= 1.0f;
				if (key == GLFW_KEY_S)
					camera.fspeed -= -1.0f;
				if (key == GLFW_KEY_A)
					camera.hspeed -= -1.0f;
				if (key == GLFW_KEY_D)
					camera.hspeed -= 1.0f;
			}
		}

	}

	virtual void cursorPosCallback(double xpos, double ypos) {
		if (true) {//panel::renderPipeline != 2) 
			mouseX = xpos;
			mouseY = ypos;
			if (leftMouseButtonPressed) {
				if (firstMouse)
				{
					lastX = clickX;
					lastY = clickY;
					firstMouse = false;
				}
				float dx = (xpos - lastX) * 0.1;
				float dy = (lastY - ypos) * 0.1;

				lastX = xpos;
				lastY = ypos;

				camera.panHorizontal(dx);
				camera.panVertical(dy);
				//std::cout << "Camera Direction: " << glm::to_string(camera.getDir()) << '\n';
			}
		}
	}

	virtual void scrollCallback(double xoffset, double yoffset) {
		//camera.cameraSpeed = yoffset;
	}

	//camera variables
	Camera camera;
	bool leftMouseButtonPressed = false;

	float mouseX = 0;
	float mouseY = 0;
	float clickX = 0;
	float clickY = 0;

	bool firstMouse = true;
	float lastX = 0;
	float lastY = 0;

	bool shouldQuit = false;

};

std::vector<glm::vec2> radiusPoint(float r, int subDivisions, int sSampleCount) {
	std::vector<glm::vec2> coords;

	// divides the radius by the subdivision, 
	// loop will sample the sRadius, it increases
	// in an even linear fashion
	float sRadius = r / (float)subDivisions;

	// Loops through the number of subdivisions
	for (int i = 0.f; i < subDivisions; i++) {
		// Samples points on the smallest circle first
		// Given the sampleCount
		for (float u = 0; u < sSampleCount; u++) {
			float x = sRadius * cos(u);
			float y = sRadius * sin(u);

			coords.push_back(glm::vec2(x, y));
		}
		// Doubles the sample count
		sSampleCount *= 2;
		// Increases the radius for the next loop
		sRadius += sRadius;
	}
	return coords;
}

int main() {
	Log::debug("Starting main");

	// WINDOW
	glfwInit();

	// Change your image/screensize here.
	int width = 1000;
	int height = 800;
	Window window(width, height, "CPSC 591 - Opal");

	//set up the image buffer for output

	GLDebug::enable();
	
	// CALLBACKS
	std::shared_ptr<Assignment5> a5 = std::make_shared<Assignment5>(); // can also update callbacks to new ones
	window.setCallbacks(a5); // can also update callbacks to new ones

	// Code to run the structure construction for voids and clusters
	glm::vec3 voxelGridSize = glm::vec3(100, 75, 25);
	VoxelGrid<clusterData> vGrid = setupGrid(voxelGridSize.x, voxelGridSize.y, voxelGridSize.z);
	vGrid.setVoidRatio(0.75f);
	distributeVoidClusters(vGrid, true);
	//distributeVoidClusterV2(vGrid, 1.f);
	trimVGrid(vGrid);
	//fillGridRandColours(vGrid);
		
	Graphics::setupOpenGL();
	Graphics::loadVoxelgrid(vGrid);

	// Sends the vGrid to the panel to display info
	panel::vGrid = vGrid;
	panel::xSize = vGrid.getDimensions().x;
	panel::ySize = vGrid.getDimensions().y;
	panel::zSize = vGrid.getDimensions().z;

	ImGuiIO& io = ImGui::GetIO();
	Graphics::clippingPlanes* clip = nullptr;
	panel::maxClipBounds = voxelGridSize;
	panel::xClipMax = voxelGridSize.x;
	panel::yClipMax = voxelGridSize.y;
	panel::zClipMax = voxelGridSize.z;

	glm::vec3 opalCenter = voxelGridSize / 2.f;

	// These are used to try and get the ray tracer working without
	// an image buffer, may need to delete
	glm::vec2 windowSize = window.getSize();
	int frag_x = 0;
	int frag_y = 0;

	// Image Buffer for ray tracer
	ImageBuffer outputImage;
	outputImage.Initialize();

	double accumulator = 0.0; // The accumulator for the remaining time
	auto previous_time = std::chrono::steady_clock::now(); // The time of the previous update
	double frameTime = 1.f;

	auto bgColour = panel::clear_color;
	// RENDER LOOP
	while (!window.shouldClose() && !a5->shouldQuit) {
		glfwPollEvents();
		if (!io.WantCaptureMouse && !io.WantCaptureKeyboard)
			a5->camera.update(frameTime);

		auto current_time = std::chrono::steady_clock::now(); // Get the current time
		auto elapsed_time = std::chrono::duration_cast<std::chrono::duration<double>>(current_time - previous_time); 
		previous_time = current_time;
		accumulator += elapsed_time.count();
		if (accumulator > 2) {
			accumulator -= 2;
		}

		glm::mat4 V = a5->camera.getView();

		if (panel::bgColourChanged) {
			bgColour = panel::clear_color;
			panel::bgColourChanged = false;
		}

		if (panel::renderPipeline < 2) {
			glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
			glDisable(GL_FRAMEBUFFER_SRGB);
			glEnable(GL_DEPTH_TEST);
			glClearColor(bgColour.x, bgColour.y, bgColour.z, 1);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		else if (panel::renderPipeline >= 2) {
			glEnable(GL_FRAMEBUFFER_SRGB);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		using namespace Graphics;

		if (panel::castPhotons) {
			panel::castPhotons = false;
			std::vector<glm::vec2> rayPoints = radiusPoint(60, 10, 1);
			//cast photons from a light into the scene

			for (int i = 0; i < rayPoints.size(); i++) { //radius of the light
					//for a sample of light wavelengths
				if (i % 500)
					std::cout << ((float)i / rayPoints.size()) * 100.f << "%\n";
				for (int wavelength = 380; wavelength <= 750; wavelength += 10) { //380 - 750
					Photon photon;
					photon.wavelength = wavelength;
					photon.pos = opalCenter + glm::vec3(opalCenter.x + rayPoints[i].x, opalCenter.y * 2.f, opalCenter.z + rayPoints[i].y);
					photon.dir = glm::vec3(0, -1, 0);
					tracePhoton(photon, vGrid);
				}
			}
			std::cout << "Photon casting finished\n";
		}

		// Orientation RGB Display
		if (panel::renderPipeline == 0) {
			glBindVertexArray(voxels_vertexArray);
			glBindBuffer(GL_ARRAY_BUFFER, voxels_instanceTransformBuffer);

			(*orientationShader).use();
			GLuint cameraUniform = glGetUniformLocation(GLuint(*orientationShader), "cameraMat");
			glUniformMatrix4fv(cameraUniform, 1, GL_FALSE, glm::value_ptr(glm::perspective(45.f, 1.f, 0.1f, 500.f) * V));

			glDrawArraysInstanced(GL_TRIANGLES, 0, 36, instancedClusterRenderData.size());
		}
		// Diffraction Grating Display
		else if (panel::renderPipeline == 1) {
			glBindVertexArray(voxels_vertexArray);
			glBindBuffer(GL_ARRAY_BUFFER, voxels_instanceTransformBuffer);

			(*gratingMaximaShader).use();
			GLuint cameraUniform = glGetUniformLocation(GLuint(*gratingMaximaShader), "cameraMat");
			glUniformMatrix4fv(cameraUniform, 1, GL_FALSE, glm::value_ptr(glm::perspective(45.f, 1.f, 0.1f, 500.f) * V));

			GLuint cameraPosUniform = glGetUniformLocation(GLuint(*gratingMaximaShader), "cameraPos");
			glUniform3fv(cameraPosUniform, 1, glm::value_ptr(a5->camera.getPos()));

			GLuint lightDirUniform = glGetUniformLocation(GLuint(*gratingMaximaShader), "lightPos");
			glUniform3fv(lightDirUniform, 1, glm::value_ptr(panel::lightPos));

			GLuint kdUniform = glGetUniformLocation(GLuint(*gratingMaximaShader), "kd");
			glUniform1f(kdUniform, panel::kd);

			GLuint ksUniform = glGetUniformLocation(GLuint(*gratingMaximaShader), "ks");
			glUniform1f(ksUniform, panel::ks);

			GLuint kaUniform = glGetUniformLocation(GLuint(*gratingMaximaShader), "ka");
			glUniform1f(kaUniform, panel::ka);

			GLuint dUniform = glGetUniformLocation(GLuint(*gratingMaximaShader), "d");
			glUniform1f(dUniform, panel::d);

			GLuint bodyColUniform = glGetUniformLocation(GLuint(*gratingMaximaShader), "in_bodyCol");
			glUniform1f(bodyColUniform, panel::bodyCol);

			GLuint centerUniform = glGetUniformLocation(GLuint(*gratingMaximaShader), "in_center");
			glUniform3fv(centerUniform, 1, glm::value_ptr(opalCenter)); 

			GLuint partDiamUniform = glGetUniformLocation(GLuint(*gratingMaximaShader), "particle_diameter");
			glUniform1f(partDiamUniform, panel::particleDiameter);
			
			GLuint flagsUniform = glGetUniformLocation(GLuint(*gratingMaximaShader), "flags");
			int flag = panel::useSanders ? 1 : 0;
			glUniform1i(flagsUniform, flag);

			glDrawArraysInstanced(GL_TRIANGLES, 0, 36, instancedClusterRenderData.size());
		}
		// Ray Trace Display
		else if (panel::renderPipeline == 2) {

			std::cout << "Raytracing\n";
			rayTraceImage(outputImage, a5->camera, panel::sample_count, panel::max_path_length, vGrid);
			outputImage.SaveToFile("Output.png");
			outputImage.Render();
			std::cout << "Raytracing finished\n";
		}
		// Photon mapping
		else if (panel::renderPipeline == 3) {
			std::cout << "photon collecting\n";
			collectPhotons(outputImage, a5->camera, 1, vGrid);
			outputImage.SaveToFile("Output.png");
			outputImage.Render();
			std::cout << "photon collection finished\n";
		}
	

		if (panel::clippingChanged) {
			std::cout << "Clipping changed\n";
			panel::clippingChanged = false;
			if (panel::useClipping) {
				clip = new Graphics::clippingPlanes;
				clip->xClipMax = panel::xClipMax;
				clip->yClipMax = panel::yClipMax;
				clip->zClipMax = panel::zClipMax;
				clip->xClipMin = panel::xClipMin;
				clip->yClipMin = panel::yClipMin;
				clip->zClipMin = panel::zClipMin;
			}
			else {
				if(clip != nullptr)
					delete clip;
				clip = nullptr;
			}
			Graphics::loadVoxelgrid(vGrid, clip);
		}

		if (panel::pipelineChanged) {
			bool fillInide = panel::renderPipeline == 0 ? true : false;
			Graphics::loadVoxelgrid(vGrid, clip, fillInide);
			panel::pipelineChanged = false;
		}

		if (panel::camSpeedChanged) {
			a5->camera.cameraSpeed = panel::camSpeed;
			panel::camSpeedChanged = false;
		}
		//a5->outputImage.Render();
		panel::updateMenu();
		window.swapBuffers();
	}

	// Save image to file:
	//a5->outputImage.SaveToFile("foo.png");

	glfwTerminate();
	return 0;
}
