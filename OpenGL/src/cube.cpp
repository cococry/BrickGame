#include "cube.h"

#include <iostream>
#include "render_state.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

Cube::Cube(const glm::vec3& position, const glm::vec3& scale, const std::shared_ptr<Texture2D>& texture,
	const std::string& name)
{
	Position = position;
	Scale = scale;
	mTexture = texture;
	mName = name;
	Aabb.Position = position;
	Aabb.HalfSize = scale / 2.0f;

	float vertices[] = {
			-0.5f, -0.5f, -0.5f,    0.0f, 0.0f,			0.9f,
			 0.5f, -0.5f, -0.5f,    1.0f, 0.0f,			0.9f,
			 0.5f,  0.5f, -0.5f,    1.0f, 1.0f,			0.9f,
			 0.5f,  0.5f, -0.5f,    1.0f, 1.0f,			0.9f,
			-0.5f,  0.5f, -0.5f,    0.0f, 1.0f,			0.9f,
			-0.5f, -0.5f, -0.5f,    0.0f, 0.0f,			0.9f,

			-0.5f, -0.5f,  0.5f,    0.0f, 0.0f,			0.6f,
			 0.5f, -0.5f,  0.5f,    1.0f, 0.0f,			0.6f,
			 0.5f,  0.5f,  0.5f,    1.0f, 1.0f,			0.6f,
			 0.5f,  0.5f,  0.5f,    1.0f, 1.0f,			0.6f,
			-0.5f,  0.5f,  0.5f,    0.0f, 1.0f,			0.6f,
			-0.5f, -0.5f,  0.5f,    0.0f, 0.0f,			0.6f,

			-0.5f,  0.5f,  0.5f,    1.0f, 0.0f,			0.6f,
			-0.5f,  0.5f, -0.5f,    1.0f, 1.0f,			0.6f,
			-0.5f, -0.5f, -0.5f,    0.0f, 1.0f,			0.6f,
			-0.5f, -0.5f, -0.5f,    0.0f, 1.0f,			0.6f,
			-0.5f, -0.5f,  0.5f,    0.0f, 0.0f,			0.6f,
			-0.5f,  0.5f,  0.5f,    1.0f, 0.0f,			0.6f,

			 0.5f,  0.5f,  0.5f,    1.0f, 0.0f,			0.7f,
			 0.5f,  0.5f, -0.5f,    1.0f, 1.0f,			0.7f,
			 0.5f, -0.5f, -0.5f,    0.0f, 1.0f,			0.7f,
			 0.5f, -0.5f, -0.5f,    0.0f, 1.0f,			0.7f,
			 0.5f, -0.5f,  0.5f,    0.0f, 0.0f,			0.7f,
			 0.5f,  0.5f,  0.5f,    1.0f, 0.0f,			0.7f,

			-0.5f, -0.5f, -0.5f,    0.0f, 1.0f,			1.0f,
			 0.5f, -0.5f, -0.5f,    1.0f, 1.0f,			1.0f,
			 0.5f, -0.5f,  0.5f,    1.0f, 0.0f,			1.0f,
			 0.5f, -0.5f,  0.5f,    1.0f, 0.0f,			1.0f,
			-0.5f, -0.5f,  0.5f,    0.0f, 0.0f,			1.0f,
			-0.5f, -0.5f, -0.5f,    0.0f, 1.0f,			1.0f,

			-0.5f,  0.5f, -0.5f,    0.0f, 1.0f,			0.55f,
			 0.5f,  0.5f, -0.5f,    1.0f, 1.0f,			0.55f,
			 0.5f,  0.5f,  0.5f,    1.0f, 0.0f,			0.55f,
			 0.5f,  0.5f,  0.5f,    1.0f, 0.0f,			0.55f,
			-0.5f,  0.5f,  0.5f,    0.0f, 0.0f,			0.55f,
			-0.5f,  0.5f, -0.5f,    0.0f, 1.0f,			0.55f
	};

	mVertexArray = std::make_shared<VertexArray>();

	std::shared_ptr<VertexBuffer> vb = std::make_shared<VertexBuffer>(vertices, sizeof(vertices));

	vb->SetLayout({
		{ "aPosition", VertexAttributeType::Vector3 },
		{ "aTexcoord", VertexAttributeType::Vector2 },
		{ "aTexcoord", VertexAttributeType::Float }
		});

	mVertexArray->AddVertexBuffer(vb);

	RenderState::GetShader()->Bind();
	RenderState::GetShader()->UploadInt("uTexture", 0);
}

Cube::~Cube()
{
}

void Cube::Render()
{
	Aabb.Position = Position;
	glm::mat4 model = glm::translate(glm::mat4(1.0f), Position)
		* glm::scale(glm::mat4(1.0f), Scale);

	RenderState::GetShader()->Bind();
	RenderState::GetShader()->UploadMat4("uModel", model);
	mTexture->Bind(0);
	mVertexArray->Bind();
	glBindTexture(GL_TEXTURE_2D, mTexture->GetID());
	glDrawArrays(GL_TRIANGLES, 0, 36);	
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
