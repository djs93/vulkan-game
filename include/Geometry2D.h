#ifndef _H_2D_GEOMETRY_
#define _H_2D_GEOMETRY_

#include "gfc_vector.h"

typedef Vector2D Point2D;

typedef struct Line2D {
	Point2D start;
	Point2D end;
} Line2D;

/**
 * @brief create and return a line2D
 * @param start the starting position of the line
 * @param end the ending position of the line
 */
Line2D line2d(Point2D start, Point2D end);

/**
 * @brief create and return a Point2D
 * @param x the x position of the point
 * @param y the y position of the point
 */
Point2D point2d(float x, float y);

Point2D point2d_zero();

float Length(Line2D line);
float LengthSq(Line2D line);

typedef struct Circle {
	Point2D position;
	float radius;
} Circle;

/**
 * @brief create and return a Circle
 * @param p the position of the center of the circle
 * @param r the radius of the circle
 */
Circle circle(Point2D p, float r);

typedef struct Rectangle2D {
	Point2D origin;
	Vector2D size;
} Rectangle2D;

/**
 * @brief create and return a 2d rectangle
 * @param origin the position of the origin of the rectangle
 * @param size the x and y size of the rectangle
 */
Rectangle2D rectangle2d(Point2D origin, Vector2D size);

Vector2D GetMin(Rectangle2D rect);
Vector2D GetMax(Rectangle2D rect);

Rectangle2D FromMinMax (Vector2D min, Vector2D max);

typedef struct OrientedRectangle {
	Point2D position;
	Vector2D halfExtents;
	float rotation;
} OrientedRectangle;

/**
 * @brief create and return an oriented rectangle
 * @param position the position of the center of the rectangle
 * @param halfExtents the x and y size from the center of the rectangle
 * @param rotation the rotation about the center in degrees
 */
OrientedRectangle orientedRectangle(Point2D position, Vector2D halfExtents, float rotation);

/**
* @brief checks to see if a point is on a line
* @param point the point to check
* @param line the line to search for the point on
* @return true if the point is on the line, false otherwise
*/
Bool PointOnLine (Point2D point, Line2D line);

/**
* @brief checks to see if a point is in a circle
* @param point the point to check
* @param c the circle to search for the point in
* @return true if the point is in the circle, false otherwise
*/
Bool PointInCircle(Point2D point, Circle c);

/**
* @brief checks to see if a point is in a rectangle
* @param point the point to check
* @param rectangle the rectangle to search for the point in
* @return true if the point is in the rectangle, false otherwise
*/
Bool PointInRectangle(Point2D point, Rectangle2D rectangle);

/**
* @brief checks to see if a point is in an oriented rectangle
* @param point the point to check
* @param rectangle the oriented rectangle to search for the point in
* @return true if the point is in the oriented rectangle, false otherwise
*/
Bool PointInOrientedRectangle(Point2D point, OrientedRectangle rectangle);

#endif