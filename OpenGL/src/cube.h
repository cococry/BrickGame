#pragma once

#include <glm/glm.hpp> 
#include <memory>

#include "texture.h"
#include "vertex_array.h"

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
	Cube(const glm::vec3& position, const glm::vec3& scale, const std::shared_ptr<Texture2D>& texture,
		const std::string& name = "Cube");
	~Cube();

	void Render();

	bool ColldingWithPoint(const glm::vec3& point);

	bool ColldingWithCube(const Cube& cube);

	inline const std::shared_ptr<Texture2D>& GetTexture() const
	{
		return mTexture;
	}
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

	AABB Aabb;
private:
	std::string mName;
	std::shared_ptr<Texture2D> mTexture;
	std::shared_ptr<VertexArray> mVertexArray;
};