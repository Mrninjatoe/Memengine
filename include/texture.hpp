#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>

class Texture {
public:
	const enum TextureFormat : const int {
		R8 = 0,
		RG8,
		RGB8,
		RGBA8,

		R16f,
		RG16f,
		RGB16f,
		RGBA16f,

		R32f,
		RG32f,
		RGB32f,
		RGBA32f,

		Depth16f,
		Depth24f,
		Depth32f
	};

	static GLenum toGLInternal(TextureFormat type) {
		static const GLenum translate[] = {
			GL_R8,
			GL_RG8,
			GL_RGB8,
			GL_RGBA8,

			GL_R16F,
			GL_RG16F,
			GL_RGB16F,
			GL_RGBA16F,

			GL_R32F,
			GL_RG32F,
			GL_RGB32F,
			GL_RGBA32F,

			GL_DEPTH_COMPONENT16,
			GL_DEPTH_COMPONENT24,
			GL_DEPTH_COMPONENT32F,
		};
		return translate[static_cast<int>(type)];
	}

	static GLenum toGLBase(TextureFormat type) {
		static const GLenum translate[] = {
			GL_RED,
			GL_RG,
			GL_RGB,
			GL_RGBA,

			GL_RED,
			GL_RG,
			GL_RGB,
			GL_RGBA,

			GL_RED,
			GL_RG,
			GL_RGB,
			GL_RGBA,

			GL_DEPTH_COMPONENT,
			GL_DEPTH_COMPONENT,
			GL_DEPTH_COMPONENT
		};
		return translate[static_cast<int>(type)];
	}

	static GLenum toGLDataType(TextureFormat format) {
		static const GLenum translate[] = {
			GL_UNSIGNED_BYTE,
			GL_UNSIGNED_BYTE,
			GL_UNSIGNED_BYTE,
			GL_UNSIGNED_BYTE,

			GL_FLOAT,
			GL_FLOAT,
			GL_FLOAT,
			GL_FLOAT,

			GL_FLOAT,
			GL_FLOAT,
			GL_FLOAT,
			GL_FLOAT,

			GL_UNSIGNED_SHORT,
			GL_UNSIGNED_INT,
			GL_FLOAT
		};
		return translate[static_cast<int>(format)];
	}

	Texture();
	Texture(const TextureFormat& format, const int width, const int height, void* data);
	Texture(const TextureFormat& format, const glm::ivec2& sizes, const bool& wantArray);
	~Texture();
	void bind(size_t pos, const bool& isArray = false) {
		glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + pos));
		if (!isArray)
			glBindTexture(GL_TEXTURE_2D, _texture);
		else
			glBindTexture(GL_TEXTURE_2D_ARRAY, _texture);
	}
	void bindCubemap(size_t pos) {
		glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + pos));
		glBindTexture(GL_TEXTURE_CUBE_MAP, _texture);
	}

	void initializeEmpty(const GLuint& id, const glm::ivec2& sizes, const TextureFormat& format) {
		_texture = id;
		_size = sizes;
		_format = format;
	}

	void intializeVSMTex(const glm::ivec2& sizes); // Variance Shadow Mapping Texture.

	GLuint& getID() { return _texture; }
	glm::ivec2 getSize() { return _size; }
private:
	glm::ivec2 _size;
	TextureFormat _format;
	GLuint _texture;
	
	void _setData(void* data) {
		// fix this
		glGenTextures(1, &_texture);
		//glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _texture);
		glTexStorage2D(GL_TEXTURE_2D, 1, toGLInternal(_format), _size.x, _size.y);	
		glTexSubImage2D(GL_TEXTURE_2D,
			0,
			0, 0,
			_size.x, _size.y,
			toGLBase(_format),
			toGLDataType(_format),
			data
		);
		
		//glTexImage2D(GL_TEXTURE_2D, 0, toGLInternal(_format), _size.x, _size.y, 0, toGLBase(_format), GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		float amount = 4.f;
		float supportedMax = 0.f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &supportedMax);
		amount = glm::min(amount, supportedMax);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, amount);

		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void _fboTexture2D() {
		glGenTextures(1, &_texture);
		glBindTexture(GL_TEXTURE_2D, _texture);
		if (_format == TextureFormat::Depth16f || _format == TextureFormat::Depth24f ||
			_format == TextureFormat::Depth32f) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		}
		else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		float amount = 4.f;
		float supportedMax = 0.f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &supportedMax);
		amount = glm::min(amount, supportedMax);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, amount);
		
		glTexStorage2D(GL_TEXTURE_2D, 1, toGLInternal(_format), _size.x, _size.y);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	void _fboTextureArray2D() {
		glGenTextures(1, &_texture);
		glBindTexture(GL_TEXTURE_2D_ARRAY, _texture);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Anistrophic
		float amount = 4.f;
		float supportedMax = 0.f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &supportedMax);
		amount = glm::min(amount, supportedMax);
		glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY_EXT, amount);

		glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, toGLInternal(_format), 
			_size.x, _size.y, 4);
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}
};