#include "vertex_array.h"

VertexArray::VertexArray()
{
	glCreateVertexArrays(1, &mID);
	glBindVertexArray(mID);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &mID);
}

void VertexArray::Bind()
{
	glBindVertexArray(mID);
}

void VertexArray::Unbind()
{
	glBindVertexArray(0);
}

void VertexArray::Delete()
{
	glDeleteVertexArrays(1, &mID);
}

void VertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
{
	glBindVertexArray(mID);
	vertexBuffer->Bind();

	uint32_t offset = 0, stride = 0;
	auto vertexLayout = vertexBuffer->GetLayout();
	for (uint32_t i = 0; i < vertexLayout.size(); i++)
	{
		stride += vertexLayout[i].ElementCount * sizeof(float);
	}
	for (uint32_t i = 0; i < vertexLayout.size(); i++)
	{
		glVertexAttribPointer(i, vertexLayout[i].ElementCount, GL_FLOAT,
			false, stride, (void*)(intptr_t)offset);
		glEnableVertexAttribArray(i);
		offset += vertexLayout[i].ElementCount * sizeof(float);
	}
	mVertexBuffers.push_back(vertexBuffer);
}

void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
{
	glBindVertexArray(mID);
	indexBuffer->Bind();

	mIndexBuffer = indexBuffer;
}
