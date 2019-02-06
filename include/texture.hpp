#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>

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

	GLenum toGLInternal(TextureFormat type) {
		static const GLenum translate[] = {
			GL_RED,
			GL_RG,
			GL_RGB,
			GL_RGBA,

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
			GL_DEPTH_COMPONENT32
		};
		return translate[static_cast<int>(type)];
	}

	GLenum toGLBase(TextureFormat type) {
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

	GLenum toGLDataType(TextureFormat format) {
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
	~Texture();
	void bind(size_t pos) {
		glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + pos));
		glBindTexture(GL_TEXTURE_2D, _texture);
	}

private:
	glm::ivec2 _size;
	TextureFormat _format;
	GLuint _texture;
	
	void _setData(void* data) {
		// fix this
		glGenTextures(1, &_texture);
		//glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _texture);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, _size.x, _size.y);	
		glTexSubImage2D(GL_TEXTURE_2D,
			0,
			0, 0,
			_size.x, _size.y,
			toGLBase(_format),
			toGLDataType(_format),
			data
		);
		
		//glTexImage2D(GL_TEXTURE_2D, 0, toGLInternal(_format), _size.x, _size.y, 0, toGLBase(_format), GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glGenerateMipmap(GL_TEXTURE_2D);
	}
};