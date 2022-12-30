#include "shader.h"
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
	std::string vertexSource = GetFileContent(vertexPath);
	std::string fragmentSource = GetFileContent(fragmentPath);

	uint32_t vertexShader = CompileShader(vertexSource, GL_VERTEX_SHADER);
	uint32_t fragmentShader = CompileShader(fragmentSource, GL_FRAGMENT_SHADER);
	
	LinkProgram(vertexShader, fragmentShader);
}

Shader::~Shader()
{
	glDeleteProgram(mID);
}

void Shader::Bind()
{
	glUseProgram(mID);
}

void Shader::Unbind()
{
	glUseProgram(0);
}

void Shader::Delete()
{
	glDeleteProgram(mID);
}

void Shader::UploadMat4(const std::string& name, const glm::mat4& val)
{
	int32_t location = glGetUniformLocation(mID, name.c_str());
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(val));
}

void Shader::UploadMat3(const std::string& name, const glm::mat4& val)
{
	int32_t location = glGetUniformLocation(mID, name.c_str());
	glUniformMatrix3fv(location, 1, false, glm::value_ptr(val));
}

void Shader::UploadVec4(const std::string& name, const glm::vec4& val)
{

	int32_t location = glGetUniformLocation(mID, name.c_str());
	glUniform4fv(location, 1, glm::value_ptr(val));
}

void Shader::UploadVec3(const std::string& name, const glm::vec3& val)
{
	int32_t location = glGetUniformLocation(mID, name.c_str());
	glUniform3fv(location, 1, glm::value_ptr(val));
}

void Shader::UploadVec2(const std::string& name, const glm::vec2& val)
{
	int32_t location = glGetUniformLocation(mID, name.c_str());
	glUniform2fv(location, 1, glm::value_ptr(val));
}

void Shader::UploadFloat(const std::string& name, float val)
{
	int32_t location = glGetUniformLocation(mID, name.c_str());
	glUniform1f(location, val);
}

void Shader::UploadInt(const std::string& name, int val)
{
	int32_t location = glGetUniformLocation(mID, name.c_str());
	glUniform1i(location, val);
}

void Shader::UploadIntArray(const std::string& name, int* values, uint32_t count)
{
	int32_t location = glGetUniformLocation(mID, name.c_str());
	glUniform1iv(location, count, values);
}

uint32_t Shader::CompileShader(const std::string& shaderSource, GLenum shaderType)
{
	int success;
	char infoLog[512];
	uint32_t shader;
	shader = glCreateShader(shaderType);
	const char* shaderSourceCSTR = shaderSource.c_str();
	glShaderSource(shader, 1, &shaderSourceCSTR, nullptr);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		std::string shaderStr = shaderType == GL_FRAGMENT_SHADER ? "fragment" : "vertex";
		std::cout << "Failed to compile " << shaderStr << " shader:\n" << infoLog << "\n";
	}
	return shader;
}

void Shader::LinkProgram(uint32_t vertex_shader, uint32_t fragment_shader)
{
	int success;
	char infoLog[512];

	mID = glCreateProgram();
	glAttachShader(mID, vertex_shader);
	glAttachShader(mID, fragment_shader);
	glLinkProgram(mID);

	glGetProgramiv(mID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(mID, 512, nullptr, infoLog);
		std::cout << "Failed to link shader program:\n " << infoLog << "\n";
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

std::string Shader::GetFileContent(const std::string& filepath)
{
	std::string result;
	std::ifstream in(filepath, std::ios::in | std::ios::binary);
	if (in)
	{
		in.seekg(0, std::ios::end);
		result.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&result[0], result.size());
		in.close();
	}
	else
	{
		std::cout << "Could not open shader file '" << filepath << "'.";
	}
	return result;
}
