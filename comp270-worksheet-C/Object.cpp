#include "stdafx.h"
#include "Object.h"

// Plane constructor. Params are:
//	centrePoint		The point on the plane from which the width and height limits are measured
//	n				The unit vector that is normal to the plane (in world space)
//	up				The unit vector along which the plane height is measured (in world space; should be orthogonal to the normal)
//	w, h			The width and height of the plane (zero/negative for an infinite plane)
Plane::Plane(Point3D centrePoint, Vector3D n, Vector3D up, float w, float h) :
	Object(centrePoint),
	m_hDir(up),
	m_normal(n),
	m_halfWidth(w / 2.0f),
	m_halfHeight(h / 2.0f)
{
	m_wDir = m_hDir.cross(m_normal);
}

//--------------------------------------------------------------------------------------------------------------------//

// Returns true if the ray intersects with this plane.
// Params:
//	raySrc					starting point of the ray (input)
//	rayDir					direction of the ray (input)
//	distToFirstIntersection	distance along the ray from the starting point of the first intersection with the plane (output)
bool Plane::getIntersection(const Point3D& raySrc, const Vector3D& rayDir, float& distToFirstIntersection) const
{
	// TODO: implement the ray-plane intersection test, returning true if the ray passes through the plane at a
	// point within the width/height bounds (if applicable).
	

	// THE Z AXIS IS WRONG IN THE DIAGRAM
	//  IT SAYS THE PLANE IS AT Z = 0 WHEN ITS AT Z = 20


	// FIRST : Distance along line where the thingies intersect

		// equation from slides
		float distanceAlongRay = ((m_centre - raySrc).dot(m_normal)) / (rayDir.dot(m_normal));
		if (true) {}

	// SECOND : find the intersection point, should be at z = 0

		// couldnt multiply float by Vector3D, so had to do it this awkward way
		Vector3D distanceXdirection = Vector3D();
		distanceXdirection.x = rayDir.x * distanceAlongRay;
		distanceXdirection.y = rayDir.y * distanceAlongRay;
		distanceXdirection.z = rayDir.z * distanceAlongRay;

		Point3D intersectPoint = raySrc + distanceXdirection;


	// SECOND : figure out if its within the bounds of the plane,   -5 < x < 5     -5 < y < 5

		// Width check
		if (intersectPoint.x >= m_centre.x - m_halfWidth)
		{
			if (intersectPoint.x <= m_centre.x + m_halfWidth)
			{

			}
			else return false;
		}
		else return false;

		if (intersectPoint.y >= m_centre.y - m_halfHeight)
		{
			if (intersectPoint.y <= m_centre.y + m_halfHeight)
			{
				distToFirstIntersection = distanceAlongRay;
				return true;
			}
		}

		else { return false; }



	return false;
}

//--------------------------------------------------------------------------------------------------------------------//

// Transforms the object using the given matrix.
void Plane::applyTransformation(const Matrix3D & matrix)
{
	m_centre = matrix * m_centre;
	m_hDir = matrix * m_hDir;
	m_wDir = matrix * m_wDir;
	m_normal = matrix.inverseTransform() * m_normal;
}

// Returns true if the ray intersects with this sphere.
// Params:
//	raySrc					starting point of the ray (input)
//	rayDir					direction of the ray (input)
//	distToFirstIntersection	distance along the ray from the starting point of the first intersection with the sphere (output)
bool Sphere::getIntersection(const Point3D& raySrc, const Vector3D& rayDir, float& distToFirstIntersection) const
{
	// Find the point on the ray closest to the sphere's centre
	Vector3D srcToCentre = m_centre - raySrc;
	float tc = srcToCentre.dot(rayDir);
	
	// Check whether the closest point is inside the sphere
	if (tc > 0.0f)
	{
		float distSq = srcToCentre.dot(srcToCentre) - tc * tc;
		if (distSq < m_radius2)
		{
			distToFirstIntersection = tc - sqrt(m_radius2 - distSq);
			return true;
		}
	}

	return false;
}

// Transforms the object using the given matrix.
void Sphere::applyTransformation(const Matrix3D & matrix)
{
	m_centre = matrix * m_centre;
}
