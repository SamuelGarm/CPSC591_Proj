#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include "Camera.h"
#include "ClusterVoid.h"
#include "panel.h"
#include <time.h>
#include <limits>

glm::vec3 radiance = glm::vec3(0);

struct Ray {
	glm::vec3 origin;
	glm::vec3 direction;
};

glm::vec2 seedGen();
float rand(glm::vec2 st);

std::vector<glm::vec3> createLocalFrame(glm::vec3 n, glm::vec3 T, glm::vec3 B);
glm::vec3 SampleHemisphere(glm::vec3 n, float r1, float r2);

void Material_Diffuse(
	Ray ray,
	glm::vec3 intersectPoint,
	glm::vec3 n,
	float r1,
	float r2,
	glm::vec3 fAcc,
	glm::vec3 emission,
	glm::vec3 finalCol);

void Material_Specular_Glossy(
	Ray ray,
	glm::vec3 intersectPoint,
	glm::vec3 n,
	glm::vec3 fAcc,
	glm::vec3 emission,
	glm::vec3 finalCol);

void Material_Specular_Glossy_Transparent(
	Ray ray,
	glm::vec3 intersectPoint,
	glm::vec3 n,
	glm::vec2 seed,
	glm::vec3 fAcc,
	glm::vec3 emission,
	glm::vec3 finalCol);

void apply_BRDF(
	Ray ray,
	glm::vec3 intersectPoint,
	glm::vec3 n,
	float r1,
	float r2,
	glm::vec2 seed,
	glm::vec3 fAcc,
	glm::vec3 emission,
	glm::vec3 finalCol);

glm::vec3 IntersectGrid(Ray ray, VoxelGrid<clusterData> vGrid);
glm::vec3 IntersectLight(Ray ray);

glm::vec3 CalculateRadiance(Ray ray, glm::vec2 seed,
	int max_path_length,
	VoxelGrid<clusterData> vGrid);

glm::vec3 RayTraceVoxel(Camera cam,
	int sample_count,
	int max_path_length,
	VoxelGrid<clusterData> vGrid);