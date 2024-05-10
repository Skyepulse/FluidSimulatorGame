#include "Shader.h"

#include <fstream>
#include <vector>
#include <array>

#include <glad/glad.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/type_ptr.hpp>

// temp
#include "glm/ext.hpp"
#include <iostream>

#include "../Core.h"

static GLenum ShaderTypeFromString(const std::string& type)
{
	if (type == "vertex")
		return GL_VERTEX_SHADER;
	if (type == "fragment")
		return GL_FRAGMENT_SHADER;

	CORE_ASSERT(false, "Unknown shader type");
	return 0;
}

Shader::Shader(const std::string& filepath)
{
	std::string source = ReadFile(filepath);
	auto shaderSources = PreProcess(source);
	Compile(shaderSources);

	// assets/texture.glsl
	auto lastSlash = filepath.find_last_of("/\\");
	lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
	auto lastDot = filepath.rfind(".");
	auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
	m_Name = filepath.substr(lastSlash, count);
}

Shader::Shader(const std::string& vertexFilepath, const std::string& fragmentFilepath)
{
	std::string vertexSource = ReadFile(vertexFilepath);
	std::string fragmentSource = ReadFile(fragmentFilepath);
	std::unordered_map<GLenum, std::string> shaderSources;

	shaderSources[GL_VERTEX_SHADER] = vertexSource;
	shaderSources[GL_FRAGMENT_SHADER] = fragmentSource;
	Compile(shaderSources);

	// assets/texture.glsl
	auto lastSlash = vertexFilepath.find_last_of("/\\");
	lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
	auto lastDot = vertexFilepath.rfind(".");
	auto count = lastDot == std::string::npos ? vertexFilepath.size() - lastSlash : lastDot - lastSlash;
	m_Name = vertexFilepath.substr(lastSlash, count);
}


Shader::Shader(const std::string name, const std::string& vertexSrc, const std::string& fragmentSrc)
	: m_Name(name)
{
	std::unordered_map<GLenum, std::string> sources;
	sources[GL_VERTEX_SHADER] = vertexSrc;
	sources[GL_FRAGMENT_SHADER] = fragmentSrc;
	Compile(sources);
}

Shader::~Shader()
{
	glDeleteProgram(m_RendererID);
}

std::string Shader::ReadFile(const std::string& filepath)
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
		CORE_ERROR("Could not open file '{0}'", filepath);
	}

	return result;
}

std::unordered_map<GLenum, std::string> Shader::PreProcess(const std::string& source)
{
	std::unordered_map<GLenum, std::string> shaderSources;

	char* typeToken = "#type";
	size_t typeTokenLength = std::strlen(typeToken);
	size_t pos = source.find(typeToken, 0);
	while (pos != source.npos)
	{
		size_t eol = source.find_first_of("\r\n", pos);
		CORE_ASSERT(eol != source.npos, "Synthax error");
		size_t begin = pos + typeTokenLength + 1; // There must be only one space between #type and vertex/fragment
		std::string type = source.substr(begin, eol - begin);
		CORE_ASSERT(ShaderTypeFromString(type), "Invalide shader type specified");

		size_t nextLinePos = source.find_first_of("\r\n", eol);
		CORE_ASSERT(nextLinePos != std::string::npos, "Synthax error");
		pos = source.find(typeToken, nextLinePos);

		shaderSources[ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
	}

	return shaderSources;
}

void Shader::Compile(std::unordered_map<GLenum, std::string>& shaderSources)
{
	// Get a program object.
	uint32_t program = glCreateProgram();
	CORE_ASSERT(shaderSources.size() <= 2, "We only support 2 shaders for now");
	std::array<GLenum, 2> glShaderIds;
	int glShaderIDIndex = 0;

	for (auto& kv : shaderSources)
	{
		// Create an empty vertex shader handle
		GLuint shader = glCreateShader(kv.first);

		// Send the vertex shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const GLchar* source = (const GLchar*)kv.second.c_str();
		glShaderSource(shader, 1, &source, 0);

		// Compile the vertex shader
		glCompileShader(shader);

		GLint isCompiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(shader);

			// Use the infoLog as you see fit.
			CORE_ERROR("Shader error :{0}", infoLog.data());
			CORE_ASSERT(false, "Shader compilation failure");

			// In this simple program, we'll just leave
			return;
		}

		glAttachShader(program, shader);
		glShaderIds[glShaderIDIndex++] = shader;
	}

	// Link our program
	m_RendererID = program;
	glLinkProgram(m_RendererID);

	// Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint isLinked = 0;
	glGetProgramiv(m_RendererID, GL_LINK_STATUS, (int*)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(m_RendererID, maxLength, &maxLength, &infoLog[0]);

		// We don't need the program anymore.
		glDeleteProgram(m_RendererID);
		// Don't leak shaders either.
		for(auto id : glShaderIds)
			glDeleteShader(id);

		// Use the infoLog as you see fit.
		CORE_ERROR("{0}", infoLog.data());
		CORE_ASSERT(false, "Shader linking failed");

		// In this simple program, we'll just leave
		return;
	}

	// Always detach shaders after a successful link.
	for (auto id : glShaderIds)
	{
		glDetachShader(m_RendererID, id);
		glDeleteShader(id);
	}
}

void Shader::Bind() const
{
	glUseProgram(m_RendererID);
}

void Shader::Unbind() const
{
	glUseProgram(0);
}

void Shader::SetInt(const char* name, int value)
{
	UploadUniformInt(name, value);
}

void Shader::SetFloat(const char* name, float value)
{
	UploadUniformFloat(name, value);
}

void Shader::SetFloat3(const char* name, const glm::vec3& value)
{
	UploadUniformFloat3(name, value);
}

void Shader::SetFloat4(const char* name, const glm::vec4& value)
{
	UploadUniformFloat4(name, value);
}

void Shader::SetMat4(const char* name, const glm::mat4& mat)
{
	UploadUniformMat4(name, mat);
}

void Shader::UploadUniformInt(const char* name, int value)
{
	unsigned int location = glGetUniformLocation(m_RendererID, name);
	if (location == -1)
		return;

	glUniform1i(location, value);
}

void Shader::UploadUniformFloat(const char* name, float value)
{
	unsigned int location = glGetUniformLocation(m_RendererID, name);
	if (location == -1)
		return;

	glUniform1f(location, value);
}

void Shader::UploadUniformFloat2(const char* name, const glm::vec2& values)
{
	unsigned int location = glGetUniformLocation(m_RendererID, name);
	if (location == -1)
		return;

	glUniform2f(location, values.x, values.y);
}

void Shader::UploadUniformFloat3(const char* name, const glm::vec3& values)
{
	unsigned int location = glGetUniformLocation(m_RendererID, name);
	if (location == -1)
		return;

	glUniform3f(location, values.x, values.y, values.z);
}

void Shader::UploadUniformFloat4(const char* name, const glm::vec4& values)
{
	unsigned int location = glGetUniformLocation(m_RendererID, name);
	if (location == -1)
		return;

	glUniform4f(location, values.x, values.y, values.z, values.w);
}

void Shader::UploadUniformMat3(const char* name, const glm::mat3& mat)
{
	unsigned int location = glGetUniformLocation(m_RendererID, name);
	if (location == -1)
		return;

	glUniformMatrix3fv(location, 1, false, glm::value_ptr(mat));
}

void Shader::UploadUniformMat4(const char* name, const glm::mat4& mat)
{
	unsigned int location = glGetUniformLocation(m_RendererID, name);
	if (location == -1)
		return;

	glUniformMatrix4fv(location, 1, false, glm::value_ptr(mat));
}