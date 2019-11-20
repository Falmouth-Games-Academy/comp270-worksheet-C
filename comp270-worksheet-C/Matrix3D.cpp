#include "stdafx.h"
#include "Matrix3D.h"

//--------------------------------------------------------------------------------------------------------------------//

// Returns the inverse of this transformation matrix, such that this * this->inverseTransform() gives the identity matrix.
Matrix3D Matrix3D::inverseTransform() const
{
	Matrix3D inverse;
	
	// TODO: the following code will invert a matrix that only performs a translation and z-axis flip;
	// update it to handle a general matrix representing affine transformations (rotation, translation, scale).

	inverse(0, 0) = m_[0][0];
	inverse(0, 1) = m_[1][0];
	inverse(0, 2) = m_[2][0];

	inverse(1, 0) = m_[0][1];
	inverse(1, 1) = m_[1][1];
	inverse(1, 2) = m_[2][1];

	inverse(2, 0) = m_[0][2];
	inverse(2, 1) = m_[1][2];
	inverse(2, 2) = m_[2][2];

	inverse(0, 3) = m_[0][0] * (-m_[0][3]) + m_[1][0] * (-m_[1][3]) + m_[2][0] * (-m_[2][3]);
	inverse(1, 3) = m_[0][1] * (-m_[0][3]) + m_[1][1] * (-m_[1][3]) + m_[2][1] * (-m_[2][3]);
	inverse(2, 3) = m_[0][2] * (-m_[0][3]) + m_[1][2] * (-m_[1][3]) + m_[2][2] * (-m_[2][3]);

	inverse(3, 0) = 0;
	inverse(3, 1) = 0;
	inverse(3, 2) = 0;
	inverse(3, 3) = 1.0f;

	return inverse;
}

//--------------------------------------------------------------------------------------------------------------------//

// Multiplies the components of a point/vector
void Matrix3D::multiply(float & x, float & y, float & z, float & w) const
{
	float x_ = m_[0][0] * x + m_[0][1] * y + m_[0][2] * z + m_[0][3] * w,
		y_ = m_[1][0] * x + m_[1][1] * y + m_[1][2] * z + m_[1][3] * w,
		z_ = m_[2][0] * x + m_[2][1] * y + m_[2][2] * z + m_[2][3] * w,
		w_ = m_[3][0] * x + m_[3][1] * y + m_[3][2] * z + m_[3][3] * w;
	x = x_;
	y = y_;
	z = z_;
	w = w_;
}
