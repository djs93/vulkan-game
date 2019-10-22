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

#endif