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

	/*p*/ Vector3D planePoint_w = (m_centre.asVector() - (m_wDir * m_halfWidth)) + (m_halfWidth * 0.75f); // (m_wDir * (m_halfWidth * 2.0f));
//	std::cout << "ppw x: " << planePoint_w.x << " ppw y: " << planePoint_w.y << " ppw z: " << planePoint_w.z << " sum: " << planePoint_w.sum() << std::endl;

	/*b*/ Vector3D planePoint_h = (m_centre.asVector() - (m_hDir * m_halfHeight)) + (m_halfHeight * 0.9f);// * (m_halfHeight * 2.0f)) + m_centre.asVector();
//	std::cout << "pph x: " << planePoint_h.x << " pph y: " << planePoint_h.y << " pph z: " << planePoint_h.z << " sum: " << planePoint_h.sum() << std::endl;

	float p = (m_normal.x * (planePoint_h.x - planePoint_w.x)) + (m_normal.y * (planePoint_h.y - planePoint_w.y)) + (m_normal.z * (planePoint_h.z - planePoint_w.z));
	std::cout << "t: " << p << std::endl;

	Vector3D plane = (planePoint_w - planePoint_h);
	Vector3D planN = plane *m_normal;
	float sum = planN.sum();
	std::cout << "sum: " << sum << std::endl;

	float tt = (((planePoint_h - raySrc.asVector()) * m_normal) / (rayDir * m_normal)).sum();
	//if (tt == 0)
		std::cout << "tt: " << tt << std::endl;

	return false;

//	Vector3D planePoint_w = ((m_wDir * 0.36f) * (m_halfWidth * 2.0f)) + m_centre.asVector();
//	Vector3D planePoint_h = ((m_hDir * 0.75f) * (m_halfHeight * 2.0f))+ m_centre.asVector();

//	Vector3D plane = ( planePoint_w - planePoint_h );
//	Vector3D planN = plane * m_normal;
//	float sum = planN.magnitude();// sum();

	float t = ((plane - raySrc.asVector()) * m_normal).sum();
		t /= (rayDir * m_normal).sum();

	std::cout << "X: "<< plane.x << " Y: " << plane.y << " Z: " << plane.z << " n.x: " << m_normal.x << " n.y: " << m_normal.z << " n.z: " << m_normal.z << " Sum: " << sum << std::endl;
	std::cout << "t: " << t << " rn: " << (rayDir * m_normal).sum()/*<< " tVal: " << tVal */<< std::endl;

	float tVal = 1.0f;//((m_normal * (rayDir * t)) + (raySrc.asVector() - plane) * m_normal).sum();	//????

	if (tVal == 0.0f)
	{
		std::cout << "t: " << t << " tVal: " << tVal << std::endl;
		distToFirstIntersection = t;
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
