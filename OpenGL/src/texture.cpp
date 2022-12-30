#include "texture.h"
#include <stb_image.h>
#include <iostream>

Texture2D::Texture2D(const std::string& filepath)
{
	mData.Filepath = filepath;

	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	stbi_uc* data = nullptr;
	data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);

	if (data == nullptr) 
	{
		std::cout << "Failed to load texture at location '" << filepath << "'.\n";
		return;
	}

	mData.Width = width;
	mData.Height = height;

	GLenum internalFormat = 0, dataFormat = 0;
	if (channels == 4)
	{
		internalFormat = GL_RGBA8;
		dataFormat = GL_RGBA;
	}
	if (channels == 3)
	{
		internalFormat = GL_RGB8;
		dataFormat = GL_RGB;
	}

	mData.InternalFormat = internalFormat;
	mData.DataFormat = dataFormat;


	glCreateTextures(GL_TEXTURE_2D, 1, &mID);
	glTextureStorage2D(mID, 1, internalFormat, mData.Width, mData.Height);

	glTextureParameteri(mID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(mID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTextureParameteri(mID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(mID, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTextureSubImage2D(mID, 0, 0, 0, mData.Width, mData.Height, dataFormat, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
}

Texture2D::~Texture2D()
{
	glDeleteTextures(1, &mID);
}

void Texture2D::Bind(uint32_t slot)
{
  	glBindTextureUnit(slot, mID);
}


void Texture2D::Delete()
{
	glDeleteTextures(1, &mID);
}
