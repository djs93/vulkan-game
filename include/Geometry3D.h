//this file is dedicated to all the time i killed by doing 2d geometry even though it wasnt needed. RIP.
//at least i'll have it for 2d next semester

#ifndef _H_GEOMETRY_3D_
#define _H_GEOMETRY_3D_

#include "gfc_vector.h"
#include "gfc_matrix.h"

typedef Vector3D Point3D;

typedef struct Line {
	Point3D start;
	Point3D end;
} Line;

float Length(Line line);
float LengthSq(Line line);

typedef struct Ray {
	Point3D origin;
	Vector3D direction;
} Ray;

/**
 * @brief create and return a line2D
 * @param start the starting position of the line
 * @param end the ending position of the line
 */
Ray ray(Point3D origin, Vector3D direction);

Ray FromPoints(Point3D from, Point3D to);

void NormalizeDirection(Ray ray);

typedef struct Sphere {
	Point3D position;
	float radius;
} Sphere;

/**
 * @brief create and return a Sphere
 * @param position the starting position of the center of the sphere
 * @param radius the radius of the sphere
 */
Sphere sphere(Point3D position, float radius);

typedef struct AABB {
	Point3D position;
	Vector3D size;
} AABB;

/**
 * @brief create and return an Axis-Aligned Bounding Box (AABB)
 * @param origin the starting position of the center of the box
 * @param size the size of the box (half-extents)
 */
AABB aabb(Point3D origin, Vector3D size);

Vector3D GetMin(AABB aabb);
Vector3D GetMax(AABB aabb);
AABB FromMinMax(Vector3D min, Vector3D max);

typedef struct OBB {
	Point3D position;
	Vector3D size;
	mat3 orientation;
} OBB;

/**
 * @brief create and return an Oriented Bounding Box (OBB)
 * @param origin the starting position of the center of the box
 * @param size the size of the box (half-extents)
 * @param orientation the rotation matrix of the OBB
 */
OBB obb(Point3D origin, Vector3D size, mat3 orientation);

typedef struct Plane {
	Vector3D normal;
	float distance;
} Plane;

/**
 * @brief create and return a plane
 * @param normal the normal vector to the plane
 * @param distance the distance to the plane from the origin
 */
Plane plane(Vector3D normal, float distance);

/**
* @brief used to detect if a point is on a plane
* @param point the point to test
* @param plane the place to test against
* @return 0 if point is on plane, positive if in front, negative if behind
*/
float PlaneEquation(Point3D point, Plane plane);

typedef struct Triangle {
	union {
		struct {
			Point3D a;
			Point3D b;
			Point3D c;
		};
		Point3D points[3];
		float values[9];
	};
} Triangle;

/**
* @brief create and return a triangle
* @param p1 the first point of the triangle
* @param p2 the second point of the triangle
* @param p3 the third point of the triangle
*/
Triangle triangle(Point3D p1, Point3D p2, Point3D p3);
#endif