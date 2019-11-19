#include "stdafx.h"
#include "Camera.h"
#include "Object.h"

#include <chrono>

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
	auto start_time = std::chrono::high_resolution_clock::now();	// start timer to see how long it take to gen rays

	//m_pixelRays.clear();	// really...
	// TODO: store the ray direction (in camera space through each pixel of the subdivided view plane,
	// and store it at an appropriate index of m_pixelRays

	// Get half the amount of samples so we can get the value in the range of -halfWidth, +halfWidth
	float half_samples_x = (m_viewPlane.resolutionX / 2.0f);
	float half_samples_y = (m_viewPlane.resolutionY / 2.0f);

	// define our samples var and get the view plan distance.
	float sample_x = 0;
	float sample_y = 0;
	float sample_plane_distance = m_viewPlane.distance;	// we can do this out side of the loop's as its the sames for all samples

	for (int i = 0; i < m_viewPlane.resolutionX; i++)
	{

		if ( i == m_pixelRays.size() )
			m_pixelRays.push_back( std::vector<Vector3D>() );

		for (int j = 0; j < m_viewPlane.resolutionY; j++)
		{
			// samples in range of -halfWidth, +halfWidth
			sample_x = (i / half_samples_x - 1.0f) * m_viewPlane.halfWidth;
			sample_y = (j / half_samples_y - 1.0f) * m_viewPlane.halfHeight;
			
			// nomalize it so we only have the direction form the COV
			Vector3D normalise_ray_vector = Vector3D(sample_x, sample_y, sample_plane_distance);
			normalise_ray_vector.normalise();
			
			// Add Value to vector.
			if ( j == m_pixelRays[i].size() )
				m_pixelRays[i].push_back(normalise_ray_vector );
			else
				m_pixelRays[i][j] = normalise_ray_vector;

		}

	}
	
	// print to console the amount of time it tock to gen rays.
	auto end_time = std::chrono::high_resolution_clock::now();
	auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
	std::cout << "Ex Time: " << dur.count() << "ms" << std::endl;

}

// Computes the transformation that will take objects from world to camera coordinates
// and stores it in m_worldToCameraTransform
void Camera::updateWorldTransform()
{
	// TODO: the following code creates a transform for a camera with translation only
	// (with the view direction along the negative z-axis); update it to handle rotations, too.

	// Rotate in z,y,x order

	// rotate around the Z
	// BUG: does not rotate when postion is zero :( (probly happens to all)

	Vector3D dif = m_position.asVector() - Vector3D(1, 1, 1);	// BUG Fix... ??? ... :S 
																// ... Dammm i need to learn more about this ... :D

	Matrix3D rZ = Matrix3D();
	rZ(0, 0) = cos(m_rotation.z);
	rZ(1, 0) = -sin(m_rotation.z);
	rZ(0, 1) = sin(m_rotation.z);
	rZ(1, 1) = cos(m_rotation.z);

	Vector3D p = rZ * dif;

	// rotate around Y
	Matrix3D rY = Matrix3D();
	rY(0, 0) = cos(m_rotation.y);
	rY(2, 0) = -sin(m_rotation.y);

	rY(0, 2) = sin(m_rotation.y);
	rY(2, 2) = cos(m_rotation.y);

	p = rY * p;
	
	// rotate around X
	Matrix3D rX = Matrix3D();
	rX(1, 1) = cos(m_rotation.x);
	rX(2, 1) = -sin(m_rotation.x);

	rX(1, 2) = sin(m_rotation.x);
	rX(2, 2) = cos(m_rotation.x);

	p = rX * p;
	
	// Applie the rotation to the world to cam transform.
	m_worldToCameraTransform(0, 3) = -p.x;// +m_position.x;
	m_worldToCameraTransform(1, 3) = -p.y;// +m_position.y;
	m_worldToCameraTransform(2, 3) = p.z;// +m_position.z;
	m_worldToCameraTransform(2, 2) = -1.0f;

	

	std::cout << p.x << "," << p.y << "," << p.z << " rx: "<< rZ(1, 1) << std::endl;
	std::cout << m_position.x << "," << m_position.y << "," << m_position.z << std::endl;
	std::cout << m_rotation.x << "," << m_rotation.y << "," << m_rotation.z << std::endl;

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