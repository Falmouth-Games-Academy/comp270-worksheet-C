#include "stdafx.h"
#include "Camera.h"
#include "Object.h"

// Initialises the camera at the given position
void Camera::init(const Point3D& pos)
{
	m_position = pos;
	m_screenBuf.init(m_viewPlane.resolutionX, m_viewPlane.resolutionY);
}

// Cast rays through the view plane and set colours based on what they intersect with
const Image& Camera::updateScreenBuffer(const std::vector<Object*>& objects)
{
	if (m_screenBuf.isInitialised())
	{
		m_screenBuf.clear();

		// Make sure our cached values are up to date
		if (m_zoomChanged)
		{
			generateRays();
			m_zoomChanged = false;
		}
		if (m_worldTransformChanged)
		{
			updateWorldTransform();
			m_worldTransformChanged = false;
		}

		if (!m_pixelRays.empty())
		{
			// Transform the objects to the camera's coordinate system
			for (auto obj : objects)
				obj->applyTransformation(m_worldToCameraTransform);

			// Set the colour based on the closest object to each pixel
			Point3D origin;
			for (unsigned i = 0; i < m_viewPlane.resolutionX; ++i)
			{
				for (unsigned j = 0; j < m_viewPlane.resolutionY; ++j)
				{
					const Object* object = getClosestIntersectedObject(origin, m_pixelRays[i][j], objects);
					if (object != nullptr)
						setPixelColourFromObject(i, j, object);
				}
			}

			// Now put the objects back!
			const Matrix3D cameraToWorldTransform = m_worldToCameraTransform.inverseTransform();
			for (auto obj : objects)
				obj->applyTransformation(cameraToWorldTransform);
		}
	}
	
	return m_screenBuf;
}
//--------------------------------------------------------------------------------------------------------------------//

// Generates and stores rays from the camera through the centre of each pixel, in camera space
void Camera::generateRays()
{
	m_pixelRays.clear();
	// TODO: store the ray direction (in camera space through each pixel of the subdivided view plane,
	// and store it at an appropriate index of m_pixelRays
	float xx = 0;
	float yy = 0;
	float zz = 0;

	for (int i = 0; i < m_viewPlane.resolutionX; i++)
	{

		m_pixelRays.push_back( std::vector<Vector3D>() );

		for (int j = 0; j < m_viewPlane.resolutionY; j++)
		{
			float xx = (i / (m_viewPlane.resolutionX / 2.0f) - 1.0f) * m_viewPlane.halfWidth;
			float yy = (j / (m_viewPlane.resolutionY / 2.0f) - 1.0f) * m_viewPlane.halfHeight;
			float zz = m_viewPlane.distance;


			Vector3D vect = Vector3D(xx, yy, zz);
			vect.normalise();

			m_pixelRays[i].push_back( vect );
		}

	}
	

}

// Computes the transformation that will take objects from world to camera coordinates
// and stores it in m_worldToCameraTransform
void Camera::updateWorldTransform()
{
	// TODO: the following code creates a transform for a camera with translation only
	// (with the view direction along the negative z-axis); update it to handle rotations, too.
	m_worldToCameraTransform(0, 3) = -m_position.x;
	m_worldToCameraTransform(1, 3) = -m_position.y;
	m_worldToCameraTransform(2, 3) = m_position.z;
	m_worldToCameraTransform(2, 2) = -1.0f;
}

// Sets the colour of a given pixel on the screen buffer based on the closest object
// Params:
//	i, j	Pixel x, y coordinates
//	object	The first object that the ray from the camera through this pixel intersects
void Camera::setPixelColourFromObject(unsigned i, unsigned j, const Object * object)
{
	// TODO: update this to make the colouring more interesting!
	// You may want to pass in extra parameters (e.g. the intersection point)...
	m_screenBuf.setPixel(i, j, object->m_colour);
}

//--------------------------------------------------------------------------------------------------------------------//

// Returns a pointer to the closest object to the ray source that is intersected by the ray from the given list.
// Params:
//	raySrc	starting point of the ray (input)
//	rayDir	direction of the ray (input)
//	objects	list of pointers to objects to test (input)
const Object* Camera::getClosestIntersectedObject(const Point3D& raySrc, const Vector3D& rayDir, const std::vector<Object*>& objects) const
{
	float distToNearestObject = FLT_MAX;
	const Object* nearestObject = nullptr;
	for (unsigned objIdx = 0; objIdx < objects.size(); ++objIdx)
	{
		float distToFirstIntersection = FLT_MAX;
		if (objects[objIdx]->getIntersection(raySrc, rayDir, distToFirstIntersection)
			&& distToFirstIntersection < distToNearestObject)
		{
			nearestObject = objects[objIdx];
			distToNearestObject = distToFirstIntersection;
		}
	}

	return nearestObject;		
}