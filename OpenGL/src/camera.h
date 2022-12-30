#pragma once

#include <glm/glm.hpp>

enum class CameraDirection
{
	Forward = 0,
	Backward,
	Left,
	Right,
	Up,
	Down
};

class Camera
{
public:
	Camera() = default;
	Camera(const glm::vec3& position);

	void UpdateDirection(double dx, double dy);

	void UpdatePosition(CameraDirection dir);

	glm::mat4 GetViewMatrix() const;

	inline glm::vec3 GetPosition() const
	{
		return mPosition;
	}

	inline float GetSpeed() const
	{
		return mSpeed;
	}

	inline float GetFOV() const
	{
		return mFov;
	}

	inline glm::vec3 GetFront() const
	{
		return mCameraFront;
	}
private:
	void UpdateVectors();

	glm::vec3 mPosition, mCameraFront, mCameraUp;
	glm::vec3 mCameraRight, mWordUp;
	float mSpeed, mFov, mYaw, mPitch;

};