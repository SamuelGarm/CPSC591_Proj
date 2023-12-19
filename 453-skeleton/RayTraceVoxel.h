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
#include "intersections.h"

struct RayAndPixel {
	Ray ray;
	int x;
	int y;
};

// I commented this out and put it in the cpp file as the linker was throwing a fit
// redefinition issues - if you know how to fix, be my guest
//glm::vec3 fRadiance;

glm::vec2 seedGen(glm::vec2 fragCoord);
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
	glm::vec3& k,
	glm::vec3 &fAcc,
	glm::vec3 &emission,
	glm::vec3 &finalCol);

Intersection wholeSceneIntersect(
	Ray& ray,
	VoxelGrid<clusterData>& vGrid,
	glm::vec3 objPos,
	float& d,
	glm::vec3& emission,
	glm::vec3& fAcc,
	glm::vec3& k);

glm::vec3 CalculateRadiance(Ray &ray, glm::vec2 seed,
	int max_path_length,
	VoxelGrid<clusterData> &vGrid);


glm::vec3 RayTraceVoxelV2(
	RayAndPixel r,
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