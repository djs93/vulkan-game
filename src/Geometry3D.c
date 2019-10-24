#include "Geometry3D.h"
#define CMP(x,y) (fabsf((x)-(y))) <= FLT_EPSILON * fmaxf(1.0f, fmaxf(fabsf(x),fabsf(y))) //CMP macro for comparing floats

float Length(Line3D line)
{
	Vector3D sub;
	vector3d_sub(sub, line.start, line.end);
	return vector3d_magnitude(sub);
}

float LengthSq(Line3D line)
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

Line3D line3d(Point3D p1, Point3D p2) {
	Line3D line;
	line.start = p1;
	line.end = p2;
	return line;
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

Bool PointInSphere(Point3D point, Sphere sphere)
{
	Vector3D sub;
	vector3d_sub(sub, point, sphere.position);
	float magSq = vector3d_magnitude_squared(sub);
	float radSq = sphere.radius * sphere.radius;
	return magSq < radSq;
}

Point3D ClosestPointSphere(Sphere sphere, Point3D point)
{
	Vector3D sphereToPoint;
	vector3d_sub(sphereToPoint,point,sphere.position);
	vector3d_normalize(&sphereToPoint);
	vector3d_scale(sphereToPoint, sphereToPoint, sphere.radius);
	Point3D result;
	vector3d_add(result, sphereToPoint, sphere.position);
	return result;
}

Bool PointInAABB(Point3D point, AABB aabbox)
{
	Point3D min = GetMin(aabbox);
	Point3D max = GetMax(aabbox);
	if (point.x < min.x || point.y < min.y || point.z < min.z) {
		return false;
	}
	if (point.x > max.x || point.y > max.y || point.z > max.z) {
		return false;
	}

	return true;
}

Point3D ClosestPointAABB(AABB aabbox, Point3D point)
{
	Point3D result = point;
	Point3D min = GetMin(aabbox);
	Point3D max = GetMax(aabbox);
	result.x = (result.x < min.x) ? min.x : result.x;
	result.y = (result.y < min.x) ? min.y : result.y;
	result.z = (result.z < min.x) ? min.z : result.z;
	result.x = (result.x > max.x) ? max.x : result.x;
	result.y = (result.y > max.x) ? max.y : result.y;
	result.z = (result.z > max.x) ? max.z : result.z;

	return result;
}

Bool PointInOBB(Point3D point, OBB obbox)
{
	Vector3D dir;
	vector3d_sub(dir,point,obbox.position);
	for (int i = 0; i < 3; ++i) {
		const float* orientation = &obbox.orientation.asArray[i * 3];
		Vector3D axis = vector3d(
			orientation[0],
			orientation[1],
			orientation[2]);
		float distance = vector3d_dot_product(dir, axis);
		if (distance > obbox.size.asArray[i]) {
			return false;
		}
		if (distance < -obbox.size.asArray[i]) {
			return false;
		}
	}

	return true;
	}

Point3D ClosestPointOBB(OBB obbox, Point3D point)
{
	Point3D result = obbox.position;
	Vector3D dir;
	vector3d_sub(dir,point,obbox.position);

	for (int i = 0; i < 3; ++i) {
		const float* orientation =
			&obbox.orientation.asArray[i * 3];
		Vector3D axis = vector3d(
			orientation[0],
			orientation[1],
			orientation[2]);
		float distance = vector3d_dot_product(dir, axis);
		if (distance > obbox.size.asArray[i]) {
			distance = obbox.size.asArray[i];
		}
		if (distance < -obbox.size.asArray[i]) {
			distance = -obbox.size.asArray[i];
		}
		Vector3D scaledAxis;
		vector3d_scale(scaledAxis, axis, distance);
		vector3d_add(result,result,scaledAxis);
	}

	return result;
}

Bool PointOnPlane(Point3D point, Plane plane)
{
	float dot = vector3d_dot_product(point, plane.normal);
	return dot - plane.distance == 0.0f;
}

Point3D ClosestPointPlane(Plane plane, Point3D point)
{
	Point3D result;
	float dot = vector3d_dot_product(plane.normal, point);
	float distance = dot - plane.distance;
	vector3d_scale(result, plane.normal, distance);
	vector3d_sub(result, point, result); //point - plane.normal * distance
	return result;
}

Bool PointOnLine3D(Point3D point, Line3D line)
{
	Point3D closest = ClosestPointLine3D(line, point);
	Vector3D diff;
	vector3d_sub(diff, closest, point);
	float distanceSq = vector3d_magnitude_squared(diff);
	return distanceSq == 0.0f;
}

Point3D ClosestPointLine3D(Line3D line, Point3D point)
{
	Vector3D lVec;// Line Vector
	vector3d_sub(lVec,line.end, line.start); 
	Vector3D diff;
	vector3d_sub(diff, point, line.start);
	float t = vector3d_dot_product(diff, lVec) / vector3d_dot_product(lVec, lVec);
	t = fmaxf(t, 0.0f); // Clamp to 0
	t = fminf(t, 1.0f); // Clamp to 1
	Point3D result;
	vector3d_scale(result, lVec, t);
	vector3d_add(result, line.start, result);
	return result;
}

Bool PointOnRay(Point3D point, Ray ray)
{
	if (vector3d_equal(point,ray.origin)) {
		return true;
	}
	Vector3D norm;
	vector3d_sub(norm,point,ray.origin);
	vector3d_normalize(&norm);
	float diff = vector3d_dot_product(norm, ray.direction);
	// If BOTH vectors point in the same direction, 
	// their dot product (diff) should be 1
	return diff == 1.0f;
}

Point3D ClosestPointRay(Ray ray, Point3D point)
{
	Vector3D diff;
	vector3d_sub(diff, point, ray.origin);
	float t = vector3d_dot_product(diff, ray.direction);
	// We assume the direction of the ray is normalized
	// If for some reason the direction is not normalized
	// the below division is needed. So long as the ray 
	// direction is normalized, we don't need this divide
	// t /= vector3d_dot_product(ray.direction, ray.direction);
	t = fmaxf(t, 0.0f);
	Point3D result;
	vector3d_scale(result, ray.direction, t);
	vector3d_add(result, result, ray.origin);
	return result;
}

Bool SphereSphere(Sphere s1, Sphere s2)
{
	float radiiSum = s1.radius + s2.radius;
	Vector3D posDiff;
	vector3d_sub(posDiff, s1.position, s2.position);
	float sqDistance = vector3d_magnitude_squared(posDiff);
	return sqDistance < radiiSum * radiiSum;
}

Bool SphereAABB(Sphere sphere, AABB aabbox)
{
	Point3D closestPoint = ClosestPointAABB(aabbox, sphere.position);
	Vector3D diff;
	vector3d_sub(diff, sphere.position, closestPoint);
	float distSq = vector3d_magnitude_squared(diff);
	float radiusSq = sphere.radius * sphere.radius;
	return distSq < radiusSq;
}

Bool SphereOBB(Sphere sphere, OBB obbox)
{
	Point3D closestPoint = ClosestPointOBB(obbox, sphere.position);
	Vector3D diff;
	vector3d_sub(diff, sphere.position, closestPoint);
	float distSq = vector3d_magnitude_squared(diff);
	float radiusSq = sphere.radius * sphere.radius;
	return distSq < radiusSq;
}

Bool SpherePlane(Sphere sphere, Plane plane)
{
	Point3D closestPoint = ClosestPointPlane(plane, sphere.position);
	Vector3D diff;
	vector3d_sub(diff, sphere.position, closestPoint);
	float distSq = vector3d_magnitude_squared(diff);
	float radiusSq = sphere.radius * sphere.radius;
	return distSq < radiusSq;
}

Bool AABBAABB(AABB aabb1, AABB aabb2)
{
	Point3D aMin = GetMin(aabb1);
	Point3D aMax = GetMax(aabb1);
	Point3D bMin = GetMin(aabb2);
	Point3D bMax = GetMax(aabb2);
	return (aMin.x <= bMax.x && aMax.x >= bMin.x) &&
		(aMin.y <= bMax.y && aMax.y >= bMin.y) &&
		(aMin.z <= bMax.z && aMax.z >= bMin.z);
}

Interval GetIntervalAABB(AABB aabbox, Vector3D axis)
{
	Vector3D i = GetMin(aabbox);
	Vector3D a = GetMax(aabbox);

	Vector3D vertex[8] = {
		vector3d(i.x, a.y, a.z),
		vector3d(i.x, a.y, i.z),
		vector3d(i.x, i.y, a.z),
		vector3d(i.x, i.y, i.z),
		vector3d(a.x, a.y, a.z),
		vector3d(a.x, a.y, i.z),
		vector3d(a.x, i.y, a.z),
		vector3d(a.x, i.y, i.z)
	};
	Interval result;
	result.min = result.max = vector3d_dot_product(axis, vertex[0]);

	for (int i = 1; i < 8; ++i) {
		float projection = vector3d_dot_product(axis, vertex[i]);
		result.min = (projection < result.min) ?
			projection : result.min;
		result.max = (projection > result.max) ?
			projection : result.max;
	}

	return result;
}

Interval GetIntervalOBB(OBB obbox, Vector3D axis)
{
	Vector3D vertex[8];
	Vector3D C = obbox.position;    // OBB Center
	Vector3D E = obbox.size;    // OBB Extents
	const float* o = obbox.orientation.asArray;
	Vector3D A[] = {              // OBB Axis
		vector3d(o[0], o[1], o[2]),
		vector3d(o[3], o[4], o[5]),
		vector3d(o[6], o[7], o[8]),
	};

	Vector3D A0E0;
	vector3d_scale(A0E0, A[0], E.asArray[0]);
	Vector3D A1E1;
	vector3d_scale(A1E1, A[1], E.asArray[1]);
	Vector3D A2E2;
	vector3d_scale(A2E2, A[2], E.asArray[2]);
	//vertex[0] = C + A[0] * E[0] + A[1] * E[1] + A[2] * E[2];
	vector3d_add(vertex[0], C, A0E0);
	vector3d_add(vertex[0], vertex[0], A1E1);
	vector3d_add(vertex[0], vertex[0], A2E2);
	//vertex[1] = C - A[0] * E[0] + A[1] * E[1] + A[2] * E[2];
	vector3d_sub(vertex[1], C, A0E0);
	vector3d_add(vertex[1], vertex[1], A1E1);
	vector3d_add(vertex[1], vertex[1], A2E2);
	//vertex[2] = C + A[0] * E[0] - A[1] * E[1] + A[2] * E[2];
	vector3d_add(vertex[2], C, A0E0);
	vector3d_sub(vertex[2], vertex[2], A1E1);
	vector3d_add(vertex[2], vertex[2], A2E2);
	//vertex[3] = C + A[0] * E[0] + A[1] * E[1] - A[2] * E[2];
	vector3d_add(vertex[3], C, A0E0);
	vector3d_add(vertex[3], vertex[3], A1E1);
	vector3d_sub(vertex[3], vertex[3], A2E2);
	//vertex[4] = C - A[0] * E[0] - A[1] * E[1] - A[2] * E[2];
	vector3d_sub(vertex[4], C, A0E0);
	vector3d_sub(vertex[4], vertex[4], A1E1);
	vector3d_sub(vertex[4], vertex[4], A2E2);
	//vertex[5] = C + A[0] * E[0] - A[1] * E[1] - A[2] * E[2];
	vector3d_add(vertex[5], C, A0E0);
	vector3d_sub(vertex[5], vertex[5], A1E1);
	vector3d_sub(vertex[5], vertex[5], A2E2);
	//vertex[6] = C - A[0] * E[0] + A[1] * E[1] - A[2] * E[2];
	vector3d_sub(vertex[6], C, A0E0);
	vector3d_add(vertex[6], vertex[6], A1E1);
	vector3d_sub(vertex[6], vertex[6], A2E2);
	//vertex[7] = C - A[0] * E[0] - A[1] * E[1] + A[2] * E[2];
	vector3d_sub(vertex[7], C, A0E0);
	vector3d_sub(vertex[7], vertex[7], A1E1);
	vector3d_add(vertex[7], vertex[7], A2E2);
	
	Interval result;
	result.min = result.max = vector3d_dot_product(axis, vertex[0]);

	for (int i = 1; i < 8; ++i) {
		float projection = vector3d_dot_product(axis, vertex[i]);
		result.min = (projection < result.min) ?
			projection : result.min;
		result.max = (projection > result.max) ?
			projection : result.max;
	}

	return result;
}

Bool OverlapOnAxis(AABB aabbox, OBB obbox, Vector3D axis)
{
	Interval a = GetIntervalAABB(aabbox, axis);
	Interval b = GetIntervalOBB(obbox, axis);
	return ((b.min <= a.max) && (a.min <= b.max));
}

Bool AABBOBB(AABB aabbox, OBB obbox)
{
	const float* o = obbox.orientation.asArray;

	Vector3D test[15] = {
	   vector3d(1, 0, 0),          // AABB axis 1
	   vector3d(0, 1, 0),          // AABB axis 2
	   vector3d(0, 0, 1),          // AABB axis 3
	   vector3d(o[0], o[1], o[2]), // OBB axis 1
	   vector3d(o[3], o[4], o[5]), // OBB axis 2
	   vector3d(o[6], o[7], o[8])  // OBB axis 3
	   // We will fill out the remaining axis in the next step
	};

	for (int i = 0; i < 3; ++i) { // Fill out rest of axis
		vector3d_cross_product(&test[6 + i * 3 + 0],test[i], test[0]);
		vector3d_cross_product(&test[6 + i * 3 + 1],test[i], test[1]);
		vector3d_cross_product(&test[6 + i * 3 + 2],test[i], test[2]);
	}

	for (int i = 0; i < 15; ++i) {
		if (!OverlapOnAxis(aabbox, obbox, test[i])) {
			return false; // Seperating axis found
		}
	}

	return true; // Seperating axis not found
}

Bool AABBPlane(AABB aabbox, Plane plane)
{
	float pLen = aabbox.size.x * fabsf(plane.normal.x) +
		aabbox.size.y * fabsf(plane.normal.y) +
		aabbox.size.z * fabsf(plane.normal.z);
	float dot = vector3d_dot_product(plane.normal, aabbox.position);
	float dist = dot - plane.distance;
	return fabsf(dist) <= pLen;
}

Bool OverlapOnAxisOBBs(OBB obb1, OBB obb2, Vector3D axis)
{
	Interval a = GetIntervalOBB(obb1, axis);
	Interval b = GetIntervalOBB(obb1, axis);
	return ((b.min <= a.max) && (a.min <= b.max));
}

Bool OBBOBB(OBB obb1, OBB obb2)
{
	const float* o1 = obb1.orientation.asArray;
	const float* o2 = obb2.orientation.asArray;

	Vector3D test[15] = {
		vector3d(o1[0], o1[1], o1[2]),
		vector3d(o1[3], o1[4], o1[5]),
		vector3d(o1[6], o1[7], o1[8]),
		vector3d(o2[0], o2[1], o2[2]),
		vector3d(o2[3], o2[4], o2[5]),
		vector3d(o2[6], o2[7], o2[8])
	};

	for (int i = 0; i < 3; ++i) { // Fill out rest of axis
		vector3d_cross_product(&test[6 + i * 3 + 0],test[i], test[0]);
		vector3d_cross_product(&test[6 + i * 3 + 1],test[i], test[1]);
		vector3d_cross_product(&test[6 + i * 3 + 2],test[i], test[2]);
	}

	for (int i = 0; i < 15; ++i) {
		if (!OverlapOnAxisOBBs(obb1, obb2, test[i])) {
			return false; // Seperating axis found
		}
	}

	return true; // Seperating axis not found
}

Bool OBBPlane(OBB obbox, Plane plane)
{
	// Local variables for readability only
	const float* o = obbox.orientation.asArray;
	Vector3D rot[] = { // rotation / orientation
		vector3d(o[0], o[1], o[2]),
		vector3d(o[3], o[4], o[5]),
		vector3d(o[6], o[7], o[8]),
	};
	Vector3D normal = plane.normal;
	float pLen = obbox.size.x * fabsf(vector3d_dot_product(normal, rot[0])) +
		obbox.size.y * fabsf(vector3d_dot_product(normal, rot[1])) +
		obbox.size.z * fabsf(vector3d_dot_product(normal, rot[2]));
	float dot = vector3d_dot_product(plane.normal, obbox.position);
	float dist = dot - plane.distance;
	return fabsf(dist) <= pLen;
}

Bool PlanePlane(Plane plane1, Plane plane2)
{
	// Cross product returns 0 when used on parallel lines
	Vector3D d;
	vector3d_cross_product(&d,plane1.normal, plane2.normal);
	return !CMP(vector3d_dot_product(d, d), 0);
}

Bool RaycastSphere(Sphere sphere, Ray ray, RaycastResult* outResult)
{
	Vector3D e;
	vector3d_sub(e,sphere.position, ray.origin);
	float rSq = sphere.radius * sphere.radius;
	float eSq = vector3d_magnitude_squared(e);
	// ray.direction is assumed to be normalized
	float a = vector3d_dot_product(e, ray.direction);
	float bSq = eSq - (a * a);
	float f = sqrt(fabsf((rSq)- bSq));
	
	// Assume normal intersection!
	float t = a - f;

	// No collision has happened
	if (rSq - (eSq - a * a) < 0.0f) {
		return false;
	}
	// Ray starts inside the sphere
	else if (eSq < rSq) {
		// Just reverse direction
		t = a + f;
	}
	if (outResult != 0) {
		outResult->t = t;
		outResult->hit = true;
		Vector3D point;
		vector3d_scale(point, ray.direction, t);
		vector3d_add(point, ray.origin, point);
		outResult->point = point;
		Vector3D norm;
		vector3d_sub(norm, outResult->point, sphere.position);
		vector3d_normalize(&norm);
		outResult->normal = norm;
	}
	return true;
}

float RaycastAABB(AABB aabbox, Ray ray)
{
	Vector3D min = GetMin(aabbox);
	Vector3D max = GetMax(aabbox);
	float t1 = (min.x - ray.origin.x) / ray.direction.x;
	float t2 = (max.x - ray.origin.x) / ray.direction.x;
	float t3 = (min.y - ray.origin.y) / ray.direction.y;
	float t4 = (max.y - ray.origin.y) / ray.direction.y;
	float t5 = (min.z - ray.origin.z) / ray.direction.z;
	float t6 = (max.z - ray.origin.z) / ray.direction.z;
	float tmin = fmaxf(
		fmaxf(
			fminf(t1, t2),
			fminf(t3, t4)
		),
		fminf(t5, t6)
	);
	float tmax = fminf(
		fminf(
			fmaxf(t1, t2),
			fmaxf(t3, t4)
		),
		fmaxf(t5, t6)
	);
	if (tmax < 0) {
		return -1;
	}
	if (tmin > tmax) {
		return -1;
	}
	if (tmin < 0.0f) {
		return tmax;
	}
	return tmin;
}

float RaycastOBB(OBB obbox, Ray ray)
{
	const float* o = obbox.orientation.asArray;
	const float* size = obbox.size.asArray;
	// X, Y and Z axis of OBB
	Vector3D X = vector3d(o[0], o[1], o[2]);
	Vector3D Y = vector3d(o[3], o[4], o[5]);
	Vector3D Z = vector3d(o[6], o[7], o[8]);
	Vector3D p;
	vector3d_sub(p,obbox.position,ray.origin);
	Vector3D f = vector3d(
		vector3d_dot_product(X, ray.direction),
		vector3d_dot_product(Y, ray.direction),
		vector3d_dot_product(Z, ray.direction)
	);
	Vector3D e = vector3d(
		vector3d_dot_product(X, p),
		vector3d_dot_product(Y, p),
		vector3d_dot_product(Z, p)
	);
	float t[6] = { 0, 0, 0, 0, 0, 0 };
	for (int i = 0; i < 3; ++i) {
		if (CMP(f.asArray[i], 0)) {
			if (-e.asArray[i] - size[i] > 0 || -e.asArray[i] + size[i] < 0) {
				return -1;
			}
			f.asArray[i] = 0.00001f; // Avoid div by 0!
		}
		t[i * 2 + 0] = (e.asArray[i] + size[i]) / f.asArray[i]; // min
		t[i * 2 + 1] = (e.asArray[i] - size[i]) / f.asArray[i]; // max
	}
	float tmin = fmaxf(
		fmaxf(
			fminf(t[0], t[1]),
			fminf(t[2], t[3])),
		fminf(t[4], t[5])
	);
	float tmax = fminf(
		fminf(
			fmaxf(t[0], t[1]),
			fmaxf(t[2], t[3])),
		fmaxf(t[4], t[5])
	);
	if (tmax < 0) {
		return -1.0f;
	}
	if (tmin > tmax) {
		return -1.0f;
	}
	if (tmin < 0.0f) {
		return tmax;
	}

	return tmin;
}

float RaycastPlane(Plane plane, Ray ray)
{
	float nd = vector3d_dot_product(ray.direction, plane.normal);
	float pn = vector3d_dot_product(ray.origin, plane.normal);
	if (nd >= 0.0f) {
		return -1;
	}

	float t = (plane.distance - pn) / nd;
	if (t >= 0.0f) {
		return t;
	}

	return -1;
}

Bool LinetestSphere(Sphere sphere, Line3D line)
{
	Point3D closest = ClosestPointLine3D(line, sphere.position);
	Vector3D diff;
	vector3d_sub(diff, sphere.position, closest);
	float distSq = vector3d_magnitude_squared(diff);
	return distSq <= (sphere.radius * sphere.radius);
}

Bool LinetestAABB(AABB aabbox, Line3D line)
{
	Ray ray;
	ray.origin = line.start;
	vector3d_sub(ray.direction, line.end, line.start);
	vector3d_normalize(&ray.direction);
	float t = RaycastAABB(aabbox, ray);

	return t >= 0 && t * t <= LengthSq(line);
}

Bool LinetestOBB(OBB obbox, Line3D line)
{
	Ray ray;
	ray.origin = line.start;
	vector3d_sub(ray.direction, line.end, line.start);
	vector3d_normalize(&ray.direction);
	float t = RaycastOBB(obbox, ray);

	return t >= 0 && t * t <= LengthSq(line);
}

Bool LinetestPlane(Plane plane, Line3D line)
{
	Vector3D ab;
	vector3d_sub(ab,line.end,line.start);

	float nA = vector3d_dot_product(plane.normal, line.start);
	float nAB = vector3d_dot_product(plane.normal, ab);

	// If the line and plane are parallel, nAB will be 0
	// This will cause a divide by 0 exception below

	float t = (plane.distance - nA) / nAB;
	return t >= 0.0f && t <= 1.0f;
}

Bool PointInTriangle(Point3D p, Triangle t)
{
	// Move the triangle so that the point is  
	// now at the origin of the triangle
	Vector3D a;
	vector3d_sub(a,t.a,p);
	Vector3D b;
	vector3d_sub(b,t.b,p);
	Vector3D c;
	vector3d_sub(c,t.c,p);

	// The point should be moved too, so they are both
	// relative, but because we don't use p in the
	// equation anymore, we don't need it!
	// p -= p; // This would just equal the zero vector!

	Vector3D normPBC;
	vector3d_cross_product(&normPBC ,b, c); // Normal of PBC (u)
	Vector3D normPCA;
	vector3d_cross_product(&normPCA,c, a); // Normal of PCA (v)
	Vector3D normPAB;
	vector3d_cross_product(&normPAB,a, b); // Normal of PAB (w)

	// Test to see if the normals are facing 
	// the same direction, return false if not
	if (vector3d_dot_product(normPBC, normPCA) < 0.0f) {
		return false;
	}
	else if (vector3d_dot_product(normPBC, normPAB) < 0.0f) {
		return false;
	}

	// All normals facing the same way, return true
	return true;
}

Plane FromTriangle(Triangle t)
{
	Plane result;
	Vector3D ba;
	vector3d_sub(ba, t.b, t.a);
	Vector3D ca;
	vector3d_sub(ca, t.c, t.a);
	vector3d_cross_product(&result.normal, ba, ca);
	vector3d_normalize(&result.normal);
	result.distance = vector3d_dot_product(result.normal, t.a);
	return result;
}

Point3D ClosestPointTriangle(Triangle t, Point3D p)
{
	Plane plane = FromTriangle(t);
	Point3D closest = ClosestPointPlane(plane, p);

	// Closest point was inside triangle
	if (PointInTriangle(closest, t)) {
		return closest;
	}

	Point3D c1 = ClosestPointLine3D(line3d(t.a, t.b), closest); // Line AB
	Point3D c2 = ClosestPointLine3D(line3d(t.b, t.c), closest); // Line BC
	Point3D c3 = ClosestPointLine3D(line3d(t.c, t.a), closest); // Line CA

	Vector3D temp;
	vector3d_sub(temp, closest, c1);
	float magSq1 = vector3d_magnitude_squared(temp);
	vector3d_sub(temp, closest, c2);
	float magSq2 = vector3d_magnitude_squared(temp);
	vector3d_sub(temp, closest, c3);
	float magSq3 = vector3d_magnitude_squared(temp);

	if (magSq1 < magSq2 && magSq1 < magSq3) {
		return c1;
	}
	else if (magSq2 < magSq1 && magSq2 < magSq3) {
		return c2;
	}

	return c3;
}

void ResetRaycastResult(RaycastResult* outResult) {
	if (outResult != 0) {
		outResult->t = -1;
		outResult->hit = false;
		outResult->normal = vector3d(0, 0, 1);
		outResult->point = vector3d(0, 0, 0);
	}
}

Point3D Intersection(Plane p1, Plane p2, Plane p3)
{
	mat3 D;
	D._11 = p1.normal.x;
	D._12 = p2.normal.x;
	D._13 = p3.normal.x;
	D._21 = p1.normal.y;
	D._22 = p1.normal.y;
	D._23 = p1.normal.y;
	D._31 = p1.normal.z;
	D._32 = p1.normal.z;
	D._33 = p1.normal.z;

	Vector3D A = vector3d(-p1.distance, -p2.distance, -p3.distance);

	mat3 Dx = D, Dy = D, Dz = D;
	Dx._11 = A.x; Dx._12 = A.y; Dx._13 = A.z;
	Dy._21 = A.x; Dy._22 = A.y; Dy._23 = A.z;
	Dz._31 = A.x; Dz._32 = A.y; Dz._33 = A.z;

	float detD = DeterminantMat3(D);

	if (CMP(detD, 0)) {
		return vector3d(0,0,0);
	}

	float detDx = DeterminantMat3(Dx);
	float detDy = DeterminantMat3(Dy);
	float detDz = DeterminantMat3(Dz);

	return vector3d(detDx / detD, detDy / detD, detDz / detD);
}
