#include "Geometry2D.h"
#include "gfc_matrix.h"
#include <math.h>
#include "local.h"

#define CMP(x,y) (fabsf((x)-(y))) <= FLT_EPSILON * fmaxf(1.0f, fmaxf(fabsf(x),fabsf(y))) //CMP macro for comparing floats

Point2D point2d(float x, float y) {
	Point2D vec;
	vector2d_set(vec, x, y);
	return vec;
}

Point2D point2d_zero()
{
	Point2D p;
	p.x = 0.f;
	p.y = 0.f;
	return p;
}

Line2D line2d(Point2D start, Point2D end) {
	Line2D line;
	line.start = start;
	line.end = end;
	return line;
}

float LengthLine2D(Line2D line) {
	Point2D vec;
	vector2d_sub(vec, line.end, line.start);
	return vector2d_magnitude(vec);
}

float LengthSqLine2D(Line2D line) {
	Point2D vec;
	vector2d_sub(vec, line.end, line.start);
	return vector2d_magnitude_squared(vec);
}

Circle circle(Point2D p, float r) {
	Circle circ;
	circ.position = p;
	circ.radius = r;
	return circ;
}

Rectangle2D rectangle2d(Point2D origin, Vector2D size) {
	Rectangle2D rect;
	rect.origin = origin;
	rect.size = size;
	return rect;
}

Vector2D GetMinRect(Rectangle2D rect) {
	Vector2D p1 = rect.origin;
	Vector2D p2;
	vector2d_add(p2,rect.origin,rect.size);

	return vector2d(fminf(p1.x, p2.x), fminf(p1.y, p2.y));
}

Vector2D GetMaxRect(Rectangle2D rect) {
	Vector2D p1 = rect.origin;
	Vector2D p2;
	vector2d_add(p2,rect.origin,rect.size);

	return vector2d(fmaxf(p1.x, p2.x), fmaxf(p1.y, p2.y));
}

Rectangle2D FromMinMaxRect(Vector2D min, Vector2D max) {
	Vector2D vec;
	vector2d_sub(vec, max, min);
	return rectangle2d(min, vec);
}

OrientedRectangle orientedRectangle(Point2D position, Vector2D halfExtents, float rotation)
{
	OrientedRectangle oRect;
	oRect.position = position;
	oRect.halfExtents = halfExtents;
	oRect.rotation = rotation;
	return oRect;
}

Bool PointOnLine(Point2D point, Line2D line)
{
	// Find the slope
	float dy = (line.end.y - line.start.y);
	float dx = (line.end.x - line.start.x);
	float M = dy / dx;
	// Find the Y-Intercept
	float B = line.start.y - M * line.start.x;
	// Check line equation
	return CMP(point.y, M * point.x + B);
}

Bool PointInCircle(Point2D point, Circle c)
{
	Line2D line = line2d(point, c.position);
	if (LengthSqLine2D(line) < c.radius * c.radius) {
		return true;
	}
	return false;
}

Bool PointInRectangle(Point2D point, Rectangle2D rectangle)
{
	Vector2D min = GetMinRect(rectangle);
	Vector2D max = GetMaxRect(rectangle);

	return  min.x <= point.x &&	min.y <= point.y &&	point.x <= max.x &&	point.y <= max.y;
}

Bool PointInOrientedRectangle(Point2D point, OrientedRectangle rectangle)
{
	Vector2D rotVector;
	Vector2D sizeVector;
	Vector2D localPoint;
	Rectangle2D localRectangle;

	vector2d_sub(rotVector,point,rectangle.position);
	float theta = -DEG2RAD*(rectangle.rotation);
	float zRotation2x2[] = {
	   cosf(theta), sinf(theta),
	   -sinf(theta), cosf(theta)
	};
	Multiply(rotVector.asArray,	vector2d(rotVector.x, rotVector.y).asArray,	1, 2, zRotation2x2, 2, 2);

	vector2d_scale(sizeVector, rectangle.halfExtents, 2.0f);
	localRectangle = rectangle2d(point2d_zero(), sizeVector);
	vector2d_add(localPoint, rotVector, rectangle.halfExtents);
	return PointInRectangle(localPoint, localRectangle);
}

Bool LineCircle(Line2D line, Circle circle)
{
	Vector2D ab;
	vector2d_sub(ab,line.end,line.start);
	Vector2D dif;
	vector2d_sub(dif, circle.position, line.start);
	float t = vector2d_dot_product(dif, ab) / vector2d_dot_product(ab, ab);
	if (t < 0.0f || t > 1.0f) {
		return false;
	}

	Point2D closestPoint;
	Vector2D scale;
	vector2d_scale(scale, ab, t);
	vector2d_add(closestPoint, line.start, scale);

	Line2D circleToClosest = line2d(circle.position, closestPoint);
	return LengthSqLine2D(circleToClosest) < circle.radius * circle.radius;
}

Bool LineRectangle(Line2D l, Rectangle2D r)
{
	if (PointInRectangle(l.start, r) ||
		PointInRectangle(l.end, r)) {
		return true;
	}

	Vector2D norm;
	vector2d_sub(norm, l.end, l.start);
	vector2d_normalize(&norm);
	norm.x = (norm.x != 0) ? 1.0f / norm.x : 0;
	norm.y = (norm.y != 0) ? 1.0f / norm.y : 0;
	Vector2D min;
	vector2d_sub(min, GetMinRect(r), l.start);
	vector2d_multiply(&min, min, norm);
	Vector2D max;
	vector2d_sub(max, GetMaxRect(r), l.start);
	vector2d_multiply(&max, max, norm);

	float tmin = fmaxf(
		fminf(min.x, max.x),
		fminf(min.y, max.y)
	);
	float tmax = fminf(
		fmaxf(min.x, max.x),
		fmaxf(min.y, max.y)
	);
	if (tmax< 0 || tmin>tmax) {
		return false;
	}
	float t = (tmin < 0.0f) ? tmax : tmin;
	return t > 0.0f && t * t < LengthSqLine2D(l);
}

Bool LineOrientedRectangle(Line2D line, OrientedRectangle rectangle)
{
	float theta = -DEG2RAD*(rectangle.rotation);
	float zRotation2x2[] = {
	   cosf(theta), sinf(theta),
	   -sinf(theta), cosf(theta)
	};
	Line2D localLine;

	Vector2D rotVector;
	vector2d_sub(rotVector, line.start, rectangle.position);
	Multiply(rotVector.asArray,
		vector2d(rotVector.x, rotVector.y).asArray,
		1, 2, zRotation2x2, 2, 2);
	vector2d_add(localLine.start,rotVector, rectangle.halfExtents);

	vector2d_sub(rotVector, line.end, rectangle.position);
	Multiply(rotVector.asArray,
		vector2d(rotVector.x, rotVector.y).asArray,
		1, 2, zRotation2x2, 2, 2);
	vector2d_add(localLine.end, rotVector, rectangle.halfExtents);

	Vector2D size;
	vector2d_scale(size, rectangle.halfExtents, 2.0f);
	Rectangle2D localRectangle = rectangle2d(point2d_zero(), size);
	return LineRectangle(localLine, localRectangle);
}
