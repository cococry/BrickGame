#include "camera.h"
#include "time.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(const glm::vec3& position)
{
	mPosition = position;
	mWordUp = glm::vec3(0.0f, 1.0f, 0.0f);
	mYaw = 90.0f;
	mPitch = -30.0f;
	mSpeed = 2.5f;
	mFov = 60.0f;
	mCameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

	UpdateVectors();
}

void Camera::UpdateDirection(double dx, double dy)
{
	mYaw += dx;
	mPitch += dy;

	if (mPitch > 89.0f)
		mPitch = 89.0f;

	else if (mPitch < -89.0f)
		mPitch = -89.0f;

	UpdateVectors();
}

void Camera::UpdatePosition(CameraDirection dir)
{
	switch (dir)
	{
	case CameraDirection::Forward:
		mPosition += mCameraFront * 1.0f;
		break;
	case CameraDirection::Backward:
		mPosition -= mCameraFront * 1.0f;
		break;
	case CameraDirection::Left:
		mPosition -= mCameraRight * 1.0f;
		break;
	case CameraDirection::Right:
		mPosition += mCameraRight * 1.0f;
		break;
	case CameraDirection::Up:
		mPosition += mCameraUp * 1.0f;
		break;
	case CameraDirection::Down:
		mPosition -= mCameraUp * 1.0f;
		break;
	default:
		break;
	}
	UpdateVectors();
}

glm::mat4 Camera::GetViewMatrix() const
{
	return glm::lookAt(mPosition, mPosition + mCameraFront, mCameraUp);
}

void Camera::UpdateVectors()
{
	glm::vec3 dir;
	dir.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
	dir.y = sin(glm::radians(mPitch));
	dir.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
	mCameraFront = glm::normalize(dir);

	mCameraRight = glm::normalize(glm::cross(mCameraFront, mWordUp));
	mCameraUp = glm::normalize(glm::cross(mCameraRight, mCameraFront));
}
