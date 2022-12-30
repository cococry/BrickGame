#pragma once

#include <string>
#include <glad/glad.h>

struct Texture2DData
{
	uint32_t Width, Height;
	std::string Filepath;
	uint32_t NumberOfChannels;
	GLenum DataFormat, InternalFormat;
};
class Texture2D 
{
public:
	Texture2D(const std::string& filepath);
	~Texture2D();

	void Bind(uint32_t slot);
	void Delete();

	inline const Texture2DData& GetData() const
	{
		return mData;
	}
private:
	uint32_t mID;
	Texture2DData mData;
	
};