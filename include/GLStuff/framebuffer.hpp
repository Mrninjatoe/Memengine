#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "texture.hpp"

class Framebuffer {
public:
	Framebuffer();
	Framebuffer(const std::string& name);
	virtual ~Framebuffer();
	Framebuffer& bind();
	Framebuffer& attachTexture(const unsigned int& pos, const glm::ivec2& size, const Texture::TextureFormat& format);
	std::shared_ptr<Texture> getTexture(const unsigned int& pos) {
		return _textureAttachments[pos];
	}
private:
	GLuint _fbo;
	glm::ivec2 _size;
	std::vector<std::shared_ptr<Texture>> _textureAttachments;
	std::string _name;
};