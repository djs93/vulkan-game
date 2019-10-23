//this file is dedicated to all the time i killed by doing 2d geometry even though it wasnt needed. RIP.
//at least i'll have it for 2d next semester

#ifndef _H_GEOMETRY_3D_
#define _H_GEOMETRY_3D_

#include "gfc_vector.h"
#include "gfc_matrix.h"
#define FLT_EPSILON 1.192092896e-07F 

typedef Vector3D Point3D;

typedef struct Line3D {
	Point3D start;
	Point3D end;
} Line3D;

Line3D line3d(Point3D p1, Point3D p2);

float Length(Line3D line);
float LengthSq(Line3D line);

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

typedef struct BVHNode {
	AABB bounds;
	struct BVHNode* children;
	int numTriangles;
	int* triangles;

} BVHNode;

typedef struct Mesh {
	int numTriangles;
	union {
		Triangle* triangles;
		Point3D* vertices;
		float* values;
	};
	BVHNode* accelerator;

} Mesh;

typedef struct Model {
	Mesh* content;
	AABB bounds;
	Vector3D position;
	Vector3D rotation;
	Bool flag;
	struct Model* parent;
} Model;

typedef struct Frustum {
	union {
		struct {
			Plane top;
			Plane bottom;
			Plane left;
			Plane right;
			Plane _near;
			Plane _far;
		};
		Plane planes[6];
	};
} Frustum;

typedef struct RaycastResult {
	Vector3D point;
	Vector3D normal;
	float t;
	Bool hit;
} RaycastResult;

typedef struct Interval {
	float min;
	float max;
} Interval;

void ResetRaycastResult(RaycastResult* outResult);

Point3D Intersection(Plane p1, Plane p2, Plane p3);
void GetCorners(Frustum f, Vector3D* outCorners);

/**
* @brief create and return a triangle
* @param p1 the first point of the triangle
* @param p2 the second point of the triangle
* @param p3 the third point of the triangle
*/
Triangle triangle(Point3D p1, Point3D p2, Point3D p3);

Bool PointInSphere(Point3D point, Sphere sphere);
Point3D ClosestPointSphere(Sphere sphere, Point3D point);

Bool PointInAABB(Point3D point, AABB aabbox);
Point3D ClosestPointAABB(AABB aabbox, Point3D point);

Bool PointInOBB(Point3D point, OBB obbox);
Point3D ClosestPointOBB(OBB obbox, Point3D point);

Bool PointOnPlane(Point3D point, Plane plane);
Point3D ClosestPointPlane(Plane plane, Point3D point);

Bool PointOnLine3D(Point3D point, Line3D line);
Point3D ClosestPointLine3D(Line3D line, Point3D point);

Bool PointOnRay(Point3D point, Ray ray);
Point3D ClosestPointRay(Ray ray, Point3D point);

//COLLISIONS (finally)
Bool SphereSphere(Sphere s1, Sphere s2);

Bool SphereAABB(Sphere sphere, AABB aabbox);
#define AABBSphere(aabbox, sphere) \ SphereAABB(Sphere, AABB)

Bool SphereOBB(Sphere sphere, OBB obbox);
#define OBBSphere(obb, sphere) \ SphereOBB(sphere, obb)

Bool SpherePlane(Sphere sphere, Plane plane);
#define PlaneSphere(plane, sphere) \ SpherePlane(sphere, plane)

Bool AABBAABB(AABB aabb1, AABB aabb2);

Interval GetIntervalAABB(AABB aabbox, Vector3D axis);
Interval GetIntervalOBB(OBB obbox, Vector3D axis);
Bool OverlapOnAxis(AABB aabbox, OBB obbox, Vector3D axis);
Bool AABBOBB(AABB aabbox, OBB obbox);
#define OBBAABB(obbox, aabbox) \ AABBOBB(aabbox, obbox)

Bool AABBPlane(AABB aabbox, Plane plane);
#define PlaneAABB(plane, aabbox) \ AABBPlane(aabbox, plane)

Bool OverlapOnAxisOBBs(OBB obb1, OBB obb2, Vector3D axis);
Bool OBBOBB(OBB obb1, OBB obb2);

Bool OBBPlane(OBB obbox, Plane plane);
#define PlaneOBB(plane, obbox) \ OBBPlane(obbox, plane)

Bool PlanePlane(Plane plane1, Plane plane2);

/**
* @brief raycast to a sphere
* @param sphere sphere to test ray against
* @param ray the ray to test intersection on sphere
* @return the time t it takes the ray to hit the sphere, -1 if it didn't hit
*/
float RaycastSphere(Sphere sphere, Ray ray);

/**
* @brief raycast to an Axis-Aligned Bounding Box
* @param aabbox the Axis-Aligned Bounding Box to test ray against
* @param ray the ray to test intersection on the AABB
* @return the time t it takes the ray to hit the AABB, -1 if it didn't hit
*/
float RaycastAABB(AABB aabbox, Ray ray);

/**
* @brief raycast to an Oriented Bounding Box
* @param obbox the Oriented Bounding Box to test ray against
* @param ray the ray to test intersection on the OBB
* @return the time t it takes the ray to hit the OBB, -1 if it didn't hit
*/
float RaycastOBB(OBB obbox, Ray ray);

/**
* @brief raycast to a plane
* @param plane the plane to test ray against
* @param ray the ray to test intersection on the plane
* @return the time t it takes the ray to hit the plane, -1 if it didn't hit
*/
float RaycastPlane(Plane plane, Ray ray);

/**
* @brief checks if a Line and a Sphere intersect
* @param sphere the sphere to test the line against
* @param line the line to test intersection on the sphere
* @return true if the line intersects the sphere, false if not
*/
Bool LinetestSphere(Sphere sphere, Line3D line);

/**
* @brief checks if a Line and an Axis-Aligned Bounding Box intersect
* @param aabbox the AABB to test the line against
* @param line the line to test intersection on the AABB
* @return true if the line intersects the AABB, false if not
*/
Bool LinetestAABB(AABB aabbox, Line3D line);

/**
* @brief checks if a Line and an Oriented Bounding Box intersect
* @param obbox the OBB to test the line against
* @param line the line to test intersection on the OBB
* @return true if the line intersects the OBB, false if not
*/
Bool LinetestOBB(OBB obbox, Line3D line);

/**
* @brief checks if a Line and a plane intersect
* @param plane the plane to test the line against
* @param line the line to test intersection on the plane
* @return true if the line intersects the plane, false if not
*/
Bool LinetestPlane(Plane plane, Line3D line);

Bool PointInTriangle(Point3D p, Triangle t);
Plane FromTriangle(Triangle t);
Point3D ClosestPointTriangle(Triangle t, Point3D p);
Bool TriangleSphere(Triangle t, Sphere s);
Bool TriangleAABB(Triangle t, AABB a);
Bool TriangleOBB(Triangle t, OBB o);
Bool TriangleTriangle(Triangle t1, Triangle t2);
Bool TriangleTriangleRobust(Triangle t1, Triangle t2);
Bool TrianglePlane(Triangle t, Plane p);

Vector3D SatCrossEdge(Vector3D a, Vector3D b, Vector3D c, Vector3D d);
Vector3D Barycentric(Point3D p, Triangle t);

void AccelerateMesh(Mesh mesh);
void SplitBVHNode(BVHNode* node, Mesh model, int depth);
void FreeBVHNode(BVHNode* node);

Bool Linetest(Mesh mesh, Line3D line);
Bool MeshSphere(Mesh mesh, Sphere sphere);
Bool MeshAABB(Mesh mesh, AABB aabb);
Bool MeshOBB(Mesh mesh, OBB obb);
Bool MeshPlane(Mesh mesh, Plane plane);
Bool MeshTriangle(Mesh mesh, Triangle triangle);
float MeshRay(Mesh mesh, Ray ray);

//Matrix4 GetWorldMatrix(Model model);
OBB GetOBB(Model model);

float ModelRay(Model model, Ray ray);
Bool Linetest(Model model, Line3D line);
Bool ModelSphere(Model model, Sphere sphere);
Bool ModelAABB(Model model, AABB aabb);
Bool ModelOBB(Model model, OBB obb);
Bool ModelPlane(Model model, Plane plane);
Bool ModelTriangle(Model model, Triangle triangle);

float Classify(AABB aabb, Plane plane);
float Classify(OBB obb, Plane plane);

Bool Intersects(Frustum f, Point3D p);
Bool Intersects(Frustum f, Sphere s);
Bool Intersects(Frustum f, AABB aabb);
Bool Intersects(Frustum f, OBB obb);

Vector3D Unproject(Vector3D viewportPoint, Vector2D viewportOrigin, Vector2D viewportSize, Matrix4 view, Matrix4 projection);
Ray GetPickRay(Vector2D viewportPoint, Vector2D viewportOrigin, Vector2D viewportSize, Matrix4 view, Matrix4 projection);

#endif