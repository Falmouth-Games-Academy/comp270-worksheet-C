#pragma once
#include "Matrix3D.h"
#include "Image.h"

class Object;

class Camera
{
public:
	void			init(const Point3D& pos);
	const Image&	updateScreenBuffer(const std::vector<Object*>& objects);

	// Change the camera's world space position
	void	translateX(float x) { m_position.x += x; m_worldTransformChanged = true; }
	void	translateY(float y) { m_position.y += y; m_worldTransformChanged = true; }
	void	translateZ(float z) { m_position.z += z; m_worldTransformChanged = true; }

	// Change the camera's world space rotation
	void	rotateX(float x) { m_rotation.x += x; m_worldTransformChanged = true; }
	void	rotateY(float y) { m_rotation.y += y; m_worldTransformChanged = true; }
	void	rotateZ(float z) { m_rotation.z += z; m_worldTransformChanged = true; }

	// Change the distance from the camera to the view plane
	void	zoom(float d) { m_viewPlane.distance += d; m_viewPlane.distance = max(1.0f, m_viewPlane.distance); m_zoomChanged = true; }

private:
	void			generateRays();
	void			updateWorldTransform();
	void			setPixelColourFromObject(unsigned i, unsigned j, const Object* object);
	const Object*	getClosestIntersectedObject(const Point3D& raySrc, const Vector3D& rayDir, const std::vector<Object*>& objects) const;
	
	Point3D		m_position = Point3D();				// The position (translation) of the camera in world space
	Vector3D	m_rotation = Vector3D();			// The Euler rotation of the camera in world space
	Matrix3D	m_worldToCameraTransform;			// The matrix representing the transformation from world to camera coordinates
	bool		m_worldTransformChanged = true;		// Flag indicating whether the camera's world transform has been updated
	bool		m_zoomChanged = true;				// Flat indicating whether the view plane distance has changed
	
	// Properties describing the view plane (framing of the picture)
	struct
	{
		float distance = 5.0f;		// Distance of the view plane from the camera (along the normal)
		float halfWidth = 5.0f;		// Half extent of the view plane along the x-axis
		float halfHeight = 5.0f;	// Half extent of the view plane along the y-axis
		unsigned resolutionX = 250, resolutionY = 250;	// The number of pixels in the x and y directions
	}	m_viewPlane;

	// Cached info for generating the image
	std::vector<std::vector<Vector3D>>	m_pixelRays;	// Stores the directions of rays passing through each pixel of the view plane
	Image	m_screenBuf;								// Stores the colours of each pixel
};