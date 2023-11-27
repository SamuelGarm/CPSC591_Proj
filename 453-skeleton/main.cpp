#define _USE_MATH_DEFINES
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include <glm/gtx/vector_query.hpp>

#include "Geometry.h"
#include "GLDebug.h"
#include "Log.h"
#include "ShaderProgram.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"
#include "imagebuffer.h"
#include "RayTrace.h"
#include "Scene.h"
#include "Lighting.h"
#include "ClusterVoid.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "PhotonMapping.h"

int hasIntersection(Scene const &scene, Ray ray, int skipID){
	for (auto &shape : scene.shapesInScene) {
		Intersection tmp = shape->getIntersection(ray);
		if(
			shape->id != skipID
			&& tmp.numberOfIntersections!=0
			&& glm::distance(tmp.point, ray.origin) > 0.00001
			&& glm::distance(tmp.point, ray.origin) < glm::distance(ray.origin, scene.lightPosition) - 0.01
		){
			return tmp.id;
		}
	}
	return -1;
}

Intersection getClosestIntersection(Scene const &scene, Ray ray, int skipID){ //get the nearest
	Intersection closestIntersection;
	float min = std::numeric_limits<float>::max();
	for(auto &shape : scene.shapesInScene) {
		if(skipID == shape->id) {
			// Sometimes you need to skip certain shapes. Useful to
			// avoid self-intersection. ;)
			continue;
		}
		Intersection p = shape->getIntersection(ray);
		float distance = glm::distance(p.point, ray.origin);
		if(p.numberOfIntersections !=0 && distance < min && distance != 0){ //the !=  condition is to allow for refraction ray casting
			min = distance;
			closestIntersection = p;
		}
	}
	return closestIntersection;
}

std::unique_ptr<glm::vec3> calculateRefractedRay(float n1, float n2, glm::vec3 I, glm::vec3 N)
{
	//https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel
	float n = (n1 / n2);
	float theta1 = acos(glm::dot(-I, N));
	float c1 = cos(theta1);
	float disc = 1-(n * n) * sin(theta1) * sin(theta1);
	if (disc < 0)
	{
		return nullptr;//there is no transmittince, only reflection!
	}
	float c2 = sqrt(disc);
	glm::vec3 T = (n * I) + (((n * c1) - c2) * N);
	std::unique_ptr<glm::vec3> temp(new glm::vec3(T));
	return temp;
}

glm::vec3 raytraceSingleRay(Scene const &scene, Ray const &ray, int level, int source_id) {
	// TODO: Part 3: Somewhere in this function you will need to add the code to determine
	//               if a given point is in shadow or not. Think carefully about what parts
	//               of the lighting equation should be used when a point is in shadow.
	// TODO: Part 4: Somewhere in this function you will need to add the code that does reflections.
	//               NOTE: The ObjectMaterial class already has a parameter to store the object's
	//               reflective properties. Use this parameter + the color coming back from the
	//               reflected array and the color from the phong shading equation.
	Intersection result = getClosestIntersection(scene, ray, source_id); //find intersection
	
	PhongReflection phong;
	phong.ray = ray;
	phong.scene = scene;
	phong.material = result.material;
	phong.intersection = result;

	glm::vec3 shadowDirection = glm::normalize(scene.lightPosition - result.point);
	Ray shadowRay = Ray(result.point, shadowDirection);
	Intersection shadowResult = getClosestIntersection(scene, shadowRay, result.id);

	
	if(result.numberOfIntersections == 0)
		return glm::vec3(0, 0, 0);

	glm::vec3 localColor = phong.I();
	if (shadowResult.numberOfIntersections != 0 && glm::distance(result.point, scene.lightPosition) > glm::distance(shadowResult.point, result.point))
	{
		localColor = phong.Ia();
	}

	//calculate transmitance through material
	glm::vec3 transmittedLight = glm::vec3(0);
	if (result.material.transmittance > 0 && false) //the &&false allows the compiler to remove all the code during compliation
	{
		std::unique_ptr<glm::vec3> refractDir = calculateRefractedRay(1, 1, ray.direction, result.normal);
		Ray refractedRay = Ray(result.point, *refractDir);
		Intersection result2 = getClosestIntersection(scene, refractedRay, -1);
		std::unique_ptr<glm::vec3> finalRefractDir = calculateRefractedRay(1, 1, *refractDir, -result2.normal);

		std::cout << ray.direction << "\n" << *finalRefractDir << "\n\n";

		if (finalRefractDir != nullptr)
		{
			Ray finalRay = Ray(result2.point, *finalRefractDir);
			Intersection res = getClosestIntersection(scene, finalRay, result2.id);
			return glm::vec3(1, 1, 1);

			//return raytraceSingleRay(scene, finalRay, level - 1, result.id);
		}
		else//not sure how to handle the total internal refraction case yet
		{
			return glm::vec3(1, 1, 0);
		}
	}

	if (level < 1) {
		return localColor + transmittedLight;
	}
	if(level >= 1 && glm::length(result.material.reflectionStrength) > 0)
	{
		Ray reflectionRay = Ray(result.point, glm::reflect(ray.direction, phong.n()));
		
		glm::vec3 reflectionColor = raytraceSingleRay(scene, reflectionRay, level - 1, result.id);
		glm::vec3 strength = result.material.reflectionStrength;
		return reflectionColor * strength + localColor + transmittedLight;
	}
	else
	{
		return localColor + transmittedLight;
	}


}

struct RayAndPixel {
	Ray ray;
	int x;
	int y;
};

std::vector<RayAndPixel> getRaysForViewpoint(Scene const &scene, ImageBuffer &image, glm::vec3 viewPoint) {
	// This is the function you must implement for part 1
	//
	// This function is responsible for creating the rays that go
	// from the viewpoint out into the scene with the appropriate direction
	// and angles to produce a perspective image.

	std::vector<RayAndPixel> rays;

	// TODO: Part 1: Currently this is casting rays in an orthographic style.
	//               You need to change this code to project them in a pinhole camera style.
	const float FOV = M_PI / 4.0f; //45 degree FOV

	float startAngle = -FOV / 2;
	float endAngle = FOV / 2;
	float hFOV = startAngle;
	float vFOV = startAngle;
	//x,y relate to the x,y of the image output pixels. This loop iterates through each pixel on the screen
	for (int x = 0; x < image.Width(); x++) {
		vFOV = startAngle;
		for (int y = 0; y < image.Height(); y++) {
			glm::vec3 direction = glm::normalize(glm::vec3(sin(hFOV), sin(vFOV), -1));
			Ray r = Ray(viewPoint, direction);
			rays.push_back({r, x, y});
			vFOV += FOV / image.Height();
		}
		hFOV += FOV / image.Width();
	}
	return rays;
}

void raytraceImage(Scene const &scene, ImageBuffer &image, glm::vec3 viewPoint) {
	// Reset the image to the current size of the screen.
	image.Initialize();

	// Get the set of rays to cast for this given image / viewpoint
	std::vector<RayAndPixel> rays = getRaysForViewpoint(scene, image, viewPoint);


	// This loops processes each ray and stores the resulting pixel in the image.
	// final color into the image at the appropriate location.
	//
	// I've written it this way, because if you're keen on this, you can
	// try and parallelize this loop to ensure that your ray tracer makes use
	// of all of your CPU cores
	//
	// Note, if you do this, you will need to be careful about how you render
	// things below too
	// std::for_each(std::begin(rays), std::end(rays), [&] (auto const &r) {
	for (auto const & r : rays) {
		glm::vec3 color = raytraceSingleRay(scene, r.ray, 4, -1);
		image.SetPixel(r.x, r.y, color);
	}
}

// EXAMPLE CALLBACKS
class Assignment5 : public CallbackInterface {

public:
	Assignment5() {
		viewPoint = glm::vec3(0, 0, 2);
		scene = initScene1();
		std::cout << "Rendering scene 1\n";
		raytraceImage(scene, outputImage, viewPoint);
		std::cout << "Finished\n";
	}

	virtual void keyCallback(int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
			shouldQuit = true;
		}

		if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
			scene = initScene1();
			std::cout << "Rendering scene 1\n";
			raytraceImage(scene, outputImage, viewPoint);
			std::cout << "Finished\n";
		}

		if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
			scene = initScene2();
			std::cout << "Rendering scene 2\n";
			raytraceImage(scene, outputImage, viewPoint);
			std::cout << "Finished\n";
		}
	}

	bool shouldQuit = false;

	ImageBuffer outputImage;
	Scene scene;
	glm::vec3 viewPoint;

};
// END EXAMPLES



int main() {
	Log::debug("Starting main");

	// WINDOW
	glfwInit();

	// Change your image/screensize here.
	int width = 800;
	int height = 800;
	Window window(width, height, "CPSC 453");

	//set up the image buffer for output
	//outpuImage.Initialize();

	GLDebug::enable();

	// CALLBACKS
	std::shared_ptr<Assignment5> a5 = std::make_shared<Assignment5>(); // can also update callbacks to new ones
	window.setCallbacks(a5); // can also update callbacks to new ones

	// Code to run the structure construction for voids and clusters
	VoxelGrid<clusterData> vGrid = setupGrid(100, 75, 25);
	distributeVoidClusters(vGrid);

	//for (int i = 0; i < vGrid.getDimensions().x; i++) {
	//	for (int j = 0; j < vGrid.getDimensions().y; j++) {
	//		for (int k = 0; k < vGrid.getDimensions().z; k++) {
	//			std::cout << vGrid.at(i, j, k).orientation.x << ","
	//				<< vGrid.at(i, j, k).orientation.y << ","
	//				<< vGrid.at(i, j, k).orientation.z << std::endl;
	//		}
	//	}
	//}

	// RENDER LOOP
	while (!window.shouldClose() && !a5->shouldQuit) {
		glfwPollEvents();

		glEnable(GL_FRAMEBUFFER_SRGB);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		a5->outputImage.Render();

		window.swapBuffers();
	}

	std::cout << "Hi !" << std::endl;

	// Save image to file:
	a5->outputImage.SaveToFile("foo.png");

	glfwTerminate();
	return 0;
}
