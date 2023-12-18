#include "intersections.h"

float planeIntersect(Ray ray, glm::vec3 p0, glm::vec3 norm) {
	float ldotn = glm::dot(ray.direction, norm);
	float numerator = glm::dot((p0 - ray.origin), norm);

	//if there is no intersection or the ray is perfectly in the plane return NO intersection
	if (ldotn == 0 || numerator == 0) {
		return 0;
	}
	return numerator / ldotn;
}

float SphereIntersect(Ray ray, glm::vec3 pos, float radius)
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
//   occurs or 0.0 if there's no intersection.
// -----------------------------------------------------------------------
{
	// Calculate the vector from the ray's origin to the sphere's center.
	glm::vec3 op = pos - ray.origin;

	// A small epsilon value for numerical stability.
	float eps = 0.01;

	// Calculate the 'b' parameter in the quadratic equation.
	float b = dot(op, ray.direction);

	// Calculate the discriminant of the quadratic equation.
	float det = b * b - dot(op, op) + radius * radius;

	// If the discriminant is negative, there is no intersection.
	if (det < 0.0) return 0.0;

	// Compute the square root of the discriminant.
	det = sqrt(det);

	// Calculate the first intersection point, t1.
	float t1 = b - det;

	// If t1 is greater than the epsilon value, it's a valid intersection.
	if (t1 > eps) return t1;

	// Calculate the second intersection point, t2.
	float t2 = b + det;

	// If t2 is greater than the epsilon value, it's a valid intersection.
	if (t2 > eps) return t2;

	// If no valid intersection points were found, return 0.0.
	return 0.0;
}