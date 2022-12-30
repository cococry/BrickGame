#include "gl_buffer_objects.h"

VertexBuffer::VertexBuffer(const void* data, uint32_t size, GLenum drawMode)
{ 
	glCreateBuffers(1, &mID);
	glBindBuffer(GL_ARRAY_BUFFER, mID);
	glBufferData(GL_ARRAY_BUFFER, size, data, drawMode);
}

VertexBuffer::VertexBuffer(uint32_t size)
{
	glCreateBuffers(1, &mID);
	glBindBuffer(GL_ARRAY_BUFFER, mID);
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &mID);
}

void VertexBuffer::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, mID);
}

void VertexBuffer::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::Delete()
{
	glDeleteBuffers(1, &mID);
}

void VertexBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
{
	glBindBuffer(GL_ARRAY_BUFFER, mID);
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

void VertexBuffer::SetLayout(const std::vector<VertexAttribute>& attributes)
{
	mVertexLayout = attributes;
}

IndexBuffer::IndexBuffer(uint32_t* data, uint32_t count, GLenum drawMode)
{
	mIndexCount = count;
	glCreateBuffers(1, &mID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), data, drawMode);
}

IndexBuffer::~IndexBuffer()
{
	glDeleteBuffers(1, &mID);
}

void IndexBuffer::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID);
}

void IndexBuffer::Unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::Delete()
{
	glDeleteBuffers(1, &mID);
}
