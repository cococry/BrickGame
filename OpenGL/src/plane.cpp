#include "plane.h"

#include "render_state.h"

#include <glm/gtc/matrix_transform.hpp>

Plane::Plane(const glm::vec3& position, const glm::vec3& scale,
	const std::shared_ptr<Texture2D>& texture)
{
	Position = position;
	Scale = scale;
	mTexture = texture;

	float vertices[] = {
	   -0.5f, 0.5f, 0.0f,	0.0f, 0.0f,
	   -0.5f, -0.5f, 0.5f,	0.0f, 1.0f,
		0.5f, -0.5f, 0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, 0.0f,  1.0f, 0.0f
	};

	uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };
	mVertexArray = std::make_shared<VertexArray>();

	std::shared_ptr<VertexBuffer> vb = std::make_shared<VertexBuffer>(vertices, sizeof(vertices));
	std::shared_ptr<IndexBuffer> ib = std::make_shared<IndexBuffer>(indices, sizeof(indices) / sizeof(uint32_t));

	vb->SetLayout({
		{ "aPosition", VertexAttributeType::Vector3 },
		{ "aTexcoord", VertexAttributeType::Vector2 }
		});

	mVertexArray->AddVertexBuffer(vb);
	mVertexArray->SetIndexBuffer(ib);

	RenderState::GetShader()->Bind();
	RenderState::GetShader()->UploadInt("uTexture", 0);
}

Plane::~Plane()
{
}

void Plane::Render()
{
	glm::mat4 model = glm::translate(glm::mat4(1.0f), Position)
		* glm::scale(glm::mat4(1.0f), Scale);

	RenderState::GetShader()->Bind();
	RenderState::GetShader()->UploadMat4("uModel", model);
	mTexture->Bind(0);
	glDrawElements(GL_TRIANGLES, mVertexArray->GetIndexBuffer()->GetIndexCount(),
		GL_UNSIGNED_INT, nullptr);
}
