#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "texture.h"
#include "vertex_array.h"

class Plane
{
public:
	Plane(const glm::vec3& position, const glm::vec3& scale, 
		const std::shared_ptr<Texture2D>& texture);
	~Plane();

	void Render();

	glm::vec3 Position, Scale;
private:
	std::shared_ptr<Texture2D> mTexture;
	std::shared_ptr<VertexArray> mVertexArray;
};