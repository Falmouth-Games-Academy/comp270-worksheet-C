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

	Vector3D pointOnPlane = m_centre.asVector();
	Vector3D otherPointOnPlane = pointOnPlane + m_hDir * (m_halfWidth*0.1f);
	Vector3D planeNorm = m_normal;

	Vector3D minVect = pointOnPlane - (m_wDir * m_halfWidth);
	Vector3D maxVect = pointOnPlane + (m_hDir * m_halfHeight);

	float zero = (pointOnPlane - otherPointOnPlane).dot(planeNorm);
	float t = (pointOnPlane - raySrc.asVector()).dot(planeNorm) / rayDir.dot( m_normal );

	Vector3D hitPosition = raySrc.asVector() + (rayDir * t);
	Vector3D hitFromCenter = m_centre.asVector() - hitPosition;

	//std::cout << "min V: x: " << minVect.x << " y: " << minVect.y << " z: " << minVect.z << " hp V: x: " << hitPosition.x << " y: " << hitPosition.y << " z: " << hitPosition.z << std::endl;

	float planeHit = ((raySrc.asVector() + (rayDir * t)) - pointOnPlane).dot(m_normal);

	// is the ray hit in range of the planes bounds.
	bool boundsZ = minVect.z <= hitPosition.z && maxVect.z >= hitPosition.z;
	bool boundsY = minVect.y <= hitPosition.y && maxVect.y >= hitPosition.y;
	bool boundsX = minVect.x <= hitPosition.x && maxVect.x >= hitPosition.x;

	if (planeHit == 0 && boundsZ && boundsY && boundsX)
	{
		distToFirstIntersection = (hitPosition - raySrc.asVector()).magnitude();
		return true;
	}

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
	//std::cout << "x: " << raySrc.x << " y: " << raySrc.y << " z: " << raySrc.z << std::endl;

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
