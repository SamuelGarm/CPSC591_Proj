#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include "Camera.h"
#include "Window.h"
#include "ClusterVoid.h"
#include "panel.h"
#include "imagebuffer.h"
#include <time.h>
#include <limits>
#include <random>

struct Ray {
	glm::vec3 origin;
	glm::vec3 direction;

	Ray(glm::vec3 point, glm::vec3 dir) {
		origin = point;
		direction = dir;
	}
	Ray() : origin(glm::vec3(0, 0, 0)), direction(glm::vec3(0, 0, 0))
	{}
};

struct RayAndPixel {
	Ray ray;
	int x;
	int y;
};

// I commented this out and put it in the cpp file as the linker was throwing a fit
// redefinition issues - if you know how to fix, be my guest
//glm::vec3 fRadiance;

glm::vec2 seedGen();
float rand(glm::vec2 st);

std::vector<glm::vec3> createLocalFrame(glm::vec3 &n, glm::vec3 &T, glm::vec3 &B);
glm::vec3 SampleHemisphere(glm::vec3 &n, float r1, float r2);

void Material_Diffuse(
	Ray &ray,
	glm::vec3 &intersectPoint,
	glm::vec3 &n,
	float r1,
	float r2,
	glm::vec3 &fAcc,
	glm::vec3 &emission,
	glm::vec3 &finalCol);

void Material_Specular_Glossy(
	Ray &ray,
	glm::vec3 &intersectPoint,
	glm::vec3 &n,
	glm::vec3 &fAcc,
	glm::vec3 &emission,
	glm::vec3 &finalCol);

void Material_Specular_Glossy_Transparent(
	Ray& ray,
	glm::vec3 &intersectPoint,
	glm::vec3 &n,
	glm::vec2 &seed,
	glm::vec3 &fAcc,
	glm::vec3 &emission,
	glm::vec3 &finalCol);

void apply_BRDF(
	Ray &ray,
	glm::vec3 &intersectPoint,
	glm::vec3 &n,
	float r1,
	float r2,
	glm::vec2 &seed,
	glm::vec3 &fAcc,
	glm::vec3 &emission,
	glm::vec3 &finalCol);

glm::vec3 IntersectGrid(Ray &ray, VoxelGrid<clusterData> &vGrid);
glm::vec3 IntersectLight(Ray &ray);
float SphereIntersect(Ray ray, glm::vec3 pos, float radius);


glm::vec3 CalculateRadiance(Ray &ray, glm::vec2 seed,
	int max_path_length,
	VoxelGrid<clusterData> &vGrid);

glm::vec3 RayTraceVoxel(
	Camera& cam,
	glm::vec2 windowSize,
	glm::vec2 fragCoord,
	int sample_count,
	int max_path_length,
	VoxelGrid<clusterData>& vGrid);

glm::vec3 RayTraceVoxelV2(
	Ray ray,
	int sample_count,
	int max_path_length,
	VoxelGrid<clusterData>& vGrid);

void rayTraceImage(
	ImageBuffer& image,
	Camera& cam,
	int sample_count,
	int max_path_length,
	VoxelGrid<clusterData>& vGrid);

glm::vec2 getRandPixel(glm::vec2& windowSize);