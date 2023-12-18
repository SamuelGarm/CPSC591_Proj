#include "intersections.h"

Intersection planeIntersect(Ray ray, glm::vec3 p0, glm::vec3 norm) {
	Intersection result;
	float ldotn = glm::dot(ray.direction, norm);
	float numerator = glm::dot((p0 - ray.origin), norm);

	//if there is no intersection or the ray is perfectly in the plane return NO intersection
	if (ldotn == 0 || numerator == 0) {
		return result;
	}

	result.isValid = true;
	result.distance = numerator / ldotn;
	result.normal = glm::dot(norm, -ray.direction) < 0 ? -norm : norm;
	result.position = ray.origin + ray.direction * result.distance;
	return result;
}

Intersection SphereIntersect(Ray ray, glm::vec3 pos, float radius)
// ----------------------------------------------------------------------
// This function, SphereIntersect, calculates the intersection points between
// a ray and a sphere.
//
// Parameters:
// - ray:    A Ray structure representing the ray to be tested for intersection.   
// - pos:    The position (center) of the sphere.
// - radius: The radius of the sphere.
//
// Returns:
// - A float representing the distance along the ray where the intersection
//   occurs
// -----------------------------------------------------------------------
{
	Intersection result;
	// Calculate the vector from the ray's origin to the sphere's center.
	glm::vec3 op = pos - ray.origin;

	// A small epsilon value for numerical stability.
	float eps = 0.01;

	// Calculate the 'b' parameter in the quadratic equation.
	float b = dot(op, ray.direction);

	// Calculate the discriminant of the quadratic equation.
	float det = b * b - dot(op, op) + radius * radius;

	// If the discriminant is negative, there is no intersection.
	if (det < 0.0) return result;

	// Compute the square root of the discriminant.
	det = sqrt(det);

	// Calculate the first intersection point, t1.
	float t1 = b - det;

	// If t1 is greater than the epsilon value, it's a valid intersection.
	if (t1 > eps) {
		result.distance = t1;
		result.isValid = true;
		result.position = ray.origin + ray.direction * t1;
		result.normal = result.position - pos;
		return result;
	}

	// Calculate the second intersection point, t2.
	float t2 = b + det;

	// If t2 is greater than the epsilon value, it's a valid intersection.
	if (t2 > eps) {
		result.distance = t2;
		result.isValid = true;
		result.position = ray.origin + ray.direction * t2;
		result.normal = result.position - pos;
		return result;
	}


	// If no valid intersection points were found, return 0.0.
	return result;
}