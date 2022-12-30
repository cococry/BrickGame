#pragma once

#include <string>
#include <vector>
#include <glad/glad.h>

enum class VertexAttributeType 
{
	Int = 0,
	Float,
	Vector2,
	Vector3,
	Vector4
}; 

struct VertexAttribute 
{
	VertexAttribute(const std::string& name, VertexAttributeType type) 
		: Type(type), Name(name)
	{
		switch (type) 
		{
		case VertexAttributeType::Int:
		case VertexAttributeType::Float:
			ElementCount = 1;
			break;
		case VertexAttributeType::Vector2:
			ElementCount = 2;
			break;
		case VertexAttributeType::Vector3:
			ElementCount = 3;
			break;
		case VertexAttributeType::Vector4:
			ElementCount = 4;
			break;
		default:
			ElementCount = 0;
			break;
		}
	}

	std::string Name;
	VertexAttributeType Type;
	uint32_t ElementCount;
};

class VertexBuffer
{
public:
	VertexBuffer(const void* data, uint32_t size, GLenum drawMode = GL_STATIC_DRAW);
	VertexBuffer(uint32_t size);
	~VertexBuffer();

	void Bind();
	void Unbind();
	void Delete();

	void SetData(const void* data, uint32_t size, uint32_t offset = 0);

	void SetLayout(const std::vector<VertexAttribute>& attributes);

	inline const std::vector<VertexAttribute>& GetLayout() const
	{
		return mVertexLayout;
	}
	inline uint32_t GetID() const
	{
		return mID;
	}
private: 
	uint32_t mID;
	std::vector<VertexAttribute> mVertexLayout;
};

class IndexBuffer
{
public:
	IndexBuffer(uint32_t* data, uint32_t count, GLenum drawMode = GL_STATIC_DRAW);
	~IndexBuffer();

	void Bind();
	void Unbind();
	void Delete();

	inline uint32_t GetIndexCount() const
	{
		return mIndexCount;
	}
	inline uint32_t GetID() const
	{
		return mID;
	}
private:
	uint32_t mID, mIndexCount;
};