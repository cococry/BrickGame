#pragma once

#include <glad/glad.h>
#include <string>
#include <assimp/scene.h>

class texture 
{
public:
	texture() = default;
	texture(const std::string& dir, const std::string& path, aiTextureType type, bool flipped = true);

	void bind();
	void unbind();
	void delete_id();

	void activate(int32_t slot = 0);

	inline int32_t get_width() const { return m_Width; }
	inline int32_t get_height() const { return m_Height; }
	inline int32_t get_channels() const { return m_NChannels; }

	inline int32_t get_id() const { return m_id; }
	aiTextureType Type;
	std::string Dir, Path;

private:
	int32_t m_Width, m_Height, m_NChannels;
	uint32_t m_id;
};