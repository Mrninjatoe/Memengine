#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "texture.hpp"

class Framebuffer {
public:
	//const enum FramebufferTextureType : const GLenum{
	//	texture2D = GL_TEXTURE_2D,
	//	textureArray2D
	//};
	const enum TextureAttachment : const int {
		positions = 0,
		normals = 1,
		colors = 2,
		depths = 3
	};

	Framebuffer();
	Framebuffer(const std::string& name);
	virtual ~Framebuffer();
	Framebuffer& bind(const GLenum& io = GL_FRAMEBUFFER);
	Framebuffer& createTexture(const unsigned int& pos, const glm::ivec2& size, 
		const Texture::TextureFormat& format, const bool& wantArray = false);
	Framebuffer& attachTexture(const unsigned int& pos, const std::shared_ptr<Texture>& tex);
	std::shared_ptr<Texture> getTexture(const unsigned int& pos) {
		return _textureAttachments[pos];
	}
	void finalize();
private:
	GLuint _fbo;
	glm::ivec2 _size;
	std::vector<std::shared_ptr<Texture>> _textureAttachments;
	std::string _name;
};