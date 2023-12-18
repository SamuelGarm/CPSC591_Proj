#pragma once
#include <algorithm>
#include <glm/glm.hpp>
#include "VoxelGrid.h"

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

float planeIntersect(Ray ray, glm::vec3 p0, glm::vec3 norm);
float SphereIntersect(Ray ray, glm::vec3 pos, float radius);


template <typename T>
float voxelGridIntersect(const Ray& ray, VoxelGrid<T>& vGrid) {
	//find which planes would be visible to the ray
	glm::vec3 raySigns = glm::vec3(glm::sign(ray.direction.x), glm::sign(ray.direction.y), glm::sign(ray.direction.z));
	float intersectionDistance = std::numeric_limits<float>::max();

	//voxel grids (right now) are always centered with the bottom corner at 0,0,0
	glm::vec3 vGridSmallCorner = glm::vec3(0, 0, 0);
	glm::vec3 vGridLargeCorner = vGrid.getDimensions();

	/*
	* X plane
	*/
	//snap the p0 points to an appropiate position such that they are in bounds of the voxel grid and (if possible) in the ray direction as close as possible
	if (raySigns.x == 1) { //if the ray is moving in the +x direction
		//start at the smallest x value allowed
		float xp0 = vGridSmallCorner.x;
		//if the plane is currently behind the ray dir
		if (xp0 < ray.origin.x) {
			//The origin is either in the voxel grid or is past it. Move the plane to the integer pos ahead of it
			xp0 = floor(ray.origin.x) + 1;
		}
		//if xp0 is in bounds of the vGrid then test for intersection with it
		if (xp0 >= 0 && xp0 <= vGridLargeCorner.x) {
			//update the intersection distance
			float d = planeIntersect(ray, glm::vec3(xp0, 0, 0), glm::vec3(1, 0, 0));
			//check that the collision point is in voxel grid bounds on all dimensions
			glm::vec3 intersectionPoint = ray.origin + ray.direction * d;
			if (d > 0 && intersectionPoint.x >= 0 && intersectionPoint.y >= 0 && intersectionPoint.z >= 0 &&
				intersectionPoint.x <= vGridLargeCorner.x && intersectionPoint.y <= vGridLargeCorner.y && intersectionPoint.z <= vGridLargeCorner.z) {
				//if the point is valid update the intersection distance
				intersectionDistance = std::min(d, intersectionDistance);
			}
		}
	}
	else {
		//start at the largest x value allowed
		float xp0 = vGridLargeCorner.x;
		//if the plane is currently behind the ray dir
		if (xp0 > ray.origin.x) {
			//The origin is either in the voxel grid or is past it. Move the plane to the integer pos ahead of it
			xp0 = ceil(ray.origin.x) - 1;
		}
		//if xp0 is in bounds of the vGrid then test for intersection with it
		if (xp0 >= 0 && xp0 <= vGridLargeCorner.x) {
			//update the intersection distance
			float d = planeIntersect(ray, glm::vec3(xp0, 0, 0), glm::vec3(1, 0, 0));
			//check that the collision point is in voxel grid bounds on all dimensions
			glm::vec3 intersectionPoint = ray.origin + ray.direction * d;
			if (d > 0 && intersectionPoint.x >= 0 && intersectionPoint.y >= 0 && intersectionPoint.z >= 0 &&
				intersectionPoint.x <= vGridLargeCorner.x && intersectionPoint.y <= vGridLargeCorner.y && intersectionPoint.z <= vGridLargeCorner.z) {
				//if the point is valid update the intersection distance
				intersectionDistance = std::min(d, intersectionDistance);
			}
		}
	}

	/*
	* Y plane
	*/
	//snap the p0 points to an appropiate position such that they are in bounds of the voxel grid and (if possible) in the ray direction as close as possible
	if (raySigns.y == 1) { //if the ray is moving in the +x direction
		//start at the smallest x value allowed
		float yp0 = vGridSmallCorner.y;
		//if the plane is currently behind the ray dir
		if (yp0 < ray.origin.y) {
			//The origin is either in the voxel grid or is past it. Move the plane to the integer pos ahead of it
			yp0 = floor(ray.origin.y) + 1;
		}
		//if xp0 is in bounds of the vGrid then test for intersection with it
		if (yp0 >= 0 && yp0 <= vGridLargeCorner.y) {
			//update the intersection distance
			float d = planeIntersect(ray, glm::vec3(0, yp0, 0), glm::vec3(0, 1, 0));
			//check that the collision point is in voxel grid bounds on all dimensions
			glm::vec3 intersectionPoint = ray.origin + ray.direction * d;
			if (d > 0 && intersectionPoint.x >= 0 && intersectionPoint.y >= 0 && intersectionPoint.z >= 0 &&
				intersectionPoint.x <= vGridLargeCorner.x && intersectionPoint.y <= vGridLargeCorner.y && intersectionPoint.z <= vGridLargeCorner.z) {
				//if the point is valid update the intersection distance
				intersectionDistance = std::min(d, intersectionDistance);
			}
		}
	}
	else {
		//start at the largest x value allowed
		float yp0 = vGridLargeCorner.y;
		//if the plane is currently behind the ray dir
		if (yp0 > ray.origin.y) {
			//The origin is either in the voxel grid or is past it. Move the plane to the integer pos ahead of it
			yp0 = ceil(ray.origin.y) - 1;
		}
		//if xp0 is in bounds of the vGrid then test for intersection with it
		if (yp0 >= 0 && yp0 <= vGridLargeCorner.y) {
			//update the intersection distance
			float d = planeIntersect(ray, glm::vec3(0, yp0, 0), glm::vec3(0, 1, 0));
			//check that the collision point is in voxel grid bounds on all dimensions
			glm::vec3 intersectionPoint = ray.origin + ray.direction * d;
			if (d > 0 && intersectionPoint.x >= 0 && intersectionPoint.y >= 0 && intersectionPoint.z >= 0 &&
				intersectionPoint.x <= vGridLargeCorner.x && intersectionPoint.y <= vGridLargeCorner.y && intersectionPoint.z <= vGridLargeCorner.z) {
				//if the point is valid update the intersection distance
				intersectionDistance = std::min(d, intersectionDistance);
			}
		}
	}

	/*
	* Z plane
	*/
	//snap the p0 points to an appropiate position such that they are in bounds of the voxel grid and (if possible) in the ray direction as close as possible
	if (raySigns.z == 1) { //if the ray is moving in the +x direction
		//start at the smallest x value allowed
		float zp0 = vGridSmallCorner.z;
		//if the plane is currently behind the ray dir
		if (zp0 < ray.origin.z) {
			//The origin is either in the voxel grid or is past it. Move the plane to the integer pos ahead of it
			zp0 = floor(ray.origin.z) + 1;
		}
		//if xp0 is in bounds of the vGrid then test for intersection with it
		if (zp0 >= 0 && zp0 <= vGridLargeCorner.z) {
			//update the intersection distance
			float d = planeIntersect(ray, glm::vec3(0, 0, zp0), glm::vec3(0, 0, 1));
			//check that the collision point is in voxel grid bounds on all dimensions
			glm::vec3 intersectionPoint = ray.origin + ray.direction * d;
			if (d > 0 && intersectionPoint.x >= 0 && intersectionPoint.y >= 0 && intersectionPoint.z >= 0 &&
				intersectionPoint.x <= vGridLargeCorner.x && intersectionPoint.y <= vGridLargeCorner.y && intersectionPoint.z <= vGridLargeCorner.z) {
				//if the point is valid update the intersection distance
				intersectionDistance = std::min(d, intersectionDistance);
			}
		}
	}
	else {
		//start at the largest x value allowed
		float zp0 = vGridLargeCorner.z;
		//if the plane is currently behind the ray dir
		if (zp0 > ray.origin.z) {
			//The origin is either in the voxel grid or is past it. Move the plane to the integer pos ahead of it
			zp0 = ceil(ray.origin.z) - 1;
		}
		//if xp0 is in bounds of the vGrid then test for intersection with it
		if (zp0 >= 0 && zp0 <= vGridLargeCorner.z) {
			//update the intersection distance
			float d = planeIntersect(ray, glm::vec3(0, 0, zp0), glm::vec3(0, 0, 1));
			//check that the collision point is in voxel grid bounds on all dimensions
			glm::vec3 intersectionPoint = ray.origin + ray.direction * d;
			if (d > 0 && intersectionPoint.x >= 0 && intersectionPoint.y >= 0 && intersectionPoint.z >= 0 &&
				intersectionPoint.x <= vGridLargeCorner.x && intersectionPoint.y <= vGridLargeCorner.y && intersectionPoint.z <= vGridLargeCorner.z) {
				//if the point is valid update the intersection distance
				intersectionDistance = std::min(d, intersectionDistance);
			}
		}
	}

	if (intersectionDistance == std::numeric_limits<float>::max())
		intersectionDistance = 0;
	return intersectionDistance;
}