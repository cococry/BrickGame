#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader
{
public:
	Shader(const std::string& vertexPath, const std::string& fragmentPath);
	~Shader();

	void Bind();
	void Unbind();
	void Delete();

	void UploadMat4(const std::string& name, const glm::mat4& val);
	void UploadMat3(const std::string& name, const glm::mat4& val);
	void UploadVec4(const std::string& name, const glm::vec4& val);
	void UploadVec3(const std::string& name, const glm::vec3& val);
	void UploadVec2(const std::string& name, const glm::vec2& val);
	void UploadFloat(const std::string& name, float val);
	void UploadInt(const std::string& name, int val);
	void UploadIntArray(const std::string& name, int* values, uint32_t count);

	inline uint32_t GetID() const
	{
		return mID;
	}
private:
	uint32_t mID;
	uint32_t CompileShader(const std::string& shaderSource, GLenum shaderType);
	void LinkProgram(uint32_t vertex_shader, uint32_t fragment_shader);
	std::string GetFileContent(const std::string& filepath);
};