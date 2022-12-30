#pragma once

#include "gl_buffer_objects.h"

#include <vector>
#include <memory>

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	void Bind();
	void Unbind();
	void Delete();

	void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);

	void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);

	inline const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const
	{
		return mVertexBuffers;
	}
	inline const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const
	{
		return mIndexBuffer;
	}
	inline uint32_t GetID() const
	{
		return mID;
	}
private:
	uint32_t mID;
	std::vector<std::shared_ptr<VertexBuffer>> mVertexBuffers;
	std::shared_ptr<IndexBuffer> mIndexBuffer;
	
};