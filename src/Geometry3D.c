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

Sphere sphere(Point3D position, float radius)
{
	Sphere s;
	s.position = position;
	s.radius = radius;
	return s;
}

AABB aabb(Point3D origin, Vector3D size)
{
	AABB a;
	a.position = origin;
	a.size = size;
	return a;
}

Vector3D GetMin(AABB aabb)
{
	Vector3D p1;
	vector3d_add(p1, aabb.position, aabb.size);
	Vector3D p2;
	vector3d_sub(p2,aabb.position,aabb.size);

	return vector3d(fminf(p1.x, p2.x),
		fminf(p1.y, p2.y),
		fminf(p1.z, p2.z));
}

Vector3D GetMax(AABB aabb)
{
	Vector3D p1;
	vector3d_add(p1, aabb.position, aabb.size);
	Vector3D p2;
	vector3d_sub(p2,aabb.position,aabb.size);

	return vector3d(fmaxf(p1.x, p2.x),
		fmaxf(p1.y, p2.y),
		fmaxf(p1.z, p2.z));
}

AABB FromMinMax(Vector3D min, Vector3D max)
{
	Vector3D add;
	Vector3D sub;
	vector3d_add(add, max, min);
	vector3d_sub(sub, max, min);
	vector3d_scale(add, add, 0.5f);
	vector3d_scale(sub, sub, 0.5f);
	return aabb(add, sub);
}

OBB obb(Point3D origin, Vector3D size, mat3 orientation)
{
	OBB box;
	box.position = origin;
	box.size = size;
	box.orientation = orientation;
	return box;
}

Plane plane(Vector3D normal, float distance)
{
	Plane p;
	p.normal = normal;
	p.distance = distance;
	return p;
}

float PlaneEquation(Point3D point, Plane plane)
{
	return vector3d_dot_product(point, plane.normal) - plane.distance;
}

Triangle triangle(Point3D p1, Point3D p2, Point3D p3)
{
	Triangle tri;
	tri.a = p1;
	tri.b = p2;
	tri.c = p3;
	return tri;
}
