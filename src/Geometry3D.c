#include "Geometry3D.h"

float Length(Line line)
{
	Vector3D sub;
	vector3d_sub(sub, line.start, line.end);
	return vector3d_magnitude(sub);
}

float LengthSq(Line line)
{
	Vector3D sub;
	vector3d_sub(sub, line.start, line.end);
	return vector3d_magnitude_squared(sub);
}

Ray ray(Point3D origin, Vector3D direciton)
{
	Ray ray;
	ray.origin = origin;
	ray.direction = direciton;
	NormalizeDirection(ray);
	return ray;
}

Ray FromPoints(Point3D from, Point3D to)
{
	Vector3D diff;
	vector3d_sub(diff, to, from);
	vector3d_normalize(&diff);
	return ray(from, diff);
}

void NormalizeDirection(Ray ray)
{
	vector3d_normalize(&ray.direction);
}
