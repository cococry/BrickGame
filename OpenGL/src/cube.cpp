#include "cube.h"

#include <iostream>
#include "render_state.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

Cube::Cube(const glm::vec3& position, const glm::vec3& col, const glm::vec3& scale, const std::shared_ptr<Model>& model,
	const std::string& name, const glm::vec3& rotation, const glm::vec3& halfScale)
{
	this->Color = glm::vec4(col, 1.0f);
	Position = position;
	Scale = scale;
	mModel = model;
	mName = name;
	Aabb.Position = position;
	if (halfScale == glm::vec3(0.0f))
		Aabb.HalfSize = scale / 2.0f;
	else
		Aabb.HalfSize = halfScale;
	Rotation = rotation;
}

Cube::~Cube()
{
}

void Cube::Render()
{
	Aabb.Position = Position;
	glm::mat4 model = glm::translate(glm::mat4(1.0f), Position) * 
		 glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)) *
		 glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)) *
		 glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)) *
		 glm::scale(glm::mat4(1.0f), Scale);

	RenderState::GetShader()->Bind();
	RenderState::GetShader()->UploadMat4("uModel", model);
	RenderState::GetShader()->UploadVec4("uColor", Color);
	std::cout << mName << ": " << Color.x << ", " << Color.y << ", " << Color.z << ", " << Color.w << "\n";
	mModel->Render();
}

bool Cube::ColldingWithPoint(const glm::vec3& point)
{
	if (point.x > Aabb.GetMin().x && point.x <
		Aabb.GetMax().x && point.y > Aabb.GetMin().y
		&& point.y < Aabb.GetMax().y && point.z >
		Aabb.GetMin().z && point.z < Aabb.GetMax().z)
	{
		return true;
	}
	return false;
}

bool Cube::ColldingWithCube(const Cube& cube)
{
	return Aabb.GetMin().x <= cube.Aabb.GetMax().x
		&& Aabb.GetMax().x >= cube.Aabb.GetMin().x
		&& Aabb.GetMin().y <= cube.Aabb.GetMax().y
		&& Aabb.GetMax().y >= cube.Aabb.GetMin().y
		&& Aabb.GetMin().z <= cube.Aabb.GetMax().z
		&& Aabb.GetMax().z >= cube.Aabb.GetMin().z;
}
