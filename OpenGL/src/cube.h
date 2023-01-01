#pragma once

#include <glm/glm.hpp> 
#include <memory>
#include "model.h"

struct AABB
{
	glm::vec3 Position;
	glm::vec3 HalfSize;

	glm::vec3 GetMin() const
	{
		glm::vec3 min = Position - HalfSize;
		return min;
	}
	glm::vec3 GetMax() const
	{
		glm::vec3 max = Position + HalfSize;
		return max;
	}
};
class Cube
{
public:
	Cube() = default;
	Cube(const glm::vec3& position, const glm::vec3& col, const glm::vec3& scale, const std::shared_ptr<Model>& model,
		const std::string& name = "Cube", const glm::vec3& rotation = glm::vec3(0.0f), const glm::vec3& halfScale = glm::vec3(0.0f));
	~Cube();

	void Render();

	bool ColldingWithPoint(const glm::vec3& point);

	bool ColldingWithCube(const Cube& cube);

	inline bool operator==(const Cube& other)
	{
		return Position == other.Position;
	}

	inline const std::string& GetName() const
	{
		return mName;
	}

	glm::vec3 Position;
	glm::vec3 Scale;
	glm::vec3 Rotation;
	glm::vec4 Color;

	AABB Aabb;
private:
	std::shared_ptr<Model> mModel;
	std::string mName;
};