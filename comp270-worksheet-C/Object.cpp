#include "stdafx.h"
#include "Object.h"
#include <math.h>

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

	if (m_normal.dot(rayDir) == 0.0)
		return false;
	else
	{
		distToFirstIntersection = (m_normal.dot(m_centre - raySrc)) / (m_normal.dot(rayDir));

		// If plane is not infinite, we check to see if ray intersects within the plane
		if (m_halfHeight != 0 && m_halfWidth != 0)
		{
			// Calculate intersection point with plane
			Vector3D a = (raySrc + rayDir * distToFirstIntersection).asVector();
			
			// Calculate bounding points of the plane
			Vector3D b = m_centre.asVector() + (m_hDir * (m_halfHeight)) + (m_wDir * (m_halfWidth));
			Vector3D c = m_centre.asVector() + (m_hDir * (m_halfHeight)) - (m_wDir * (m_halfWidth));
			Vector3D d = m_centre.asVector() - (m_hDir * (m_halfHeight)) - (m_wDir * (m_halfWidth));
			Vector3D e = m_centre.asVector() - (m_hDir * (m_halfHeight)) + (m_wDir * (m_halfWidth));

			if (b.dot(c - b) <= a.dot(c - b) <= c.dot(c - b))
			{
				if (b.dot(e - b) <= a.dot(e - b) <= e.dot(e - b))
				{
					return true;
				}
			}
			return false;
		}
		else
			return true;
	}
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
