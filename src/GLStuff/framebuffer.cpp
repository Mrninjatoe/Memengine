#include <stdio.h>
#include "engine.hpp"
#include "GLStuff/framebuffer.hpp"

Framebuffer::Framebuffer() : _name("No name"){
	glGenFramebuffers(1, &_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		printf("Successfully created a framebuffer :sunglasses:\n");
	else
		printf("Unsuccessfully created a framebuffer :sad:\n");
}

Framebuffer::Framebuffer(const std::string& name) : _name(name){
	glGenFramebuffers(1, &_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		printf("Successfully created a framebuffer :sunglasses:\n");
	else
		printf("Unsuccessfully created a framebuffer :sad:\n");
}

Framebuffer::~Framebuffer() {
	glDeleteFramebuffers(1, &_fbo);
}

Framebuffer& Framebuffer::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

	return *this;
}

Framebuffer& Framebuffer::attachTexture(const unsigned int& pos, const glm::ivec2& size, const Texture::TextureFormat& format) {
	auto attachment = std::make_shared<Texture>(format, size);

	glFramebufferTexture2D(GL_FRAMEBUFFER, static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + pos), 
		GL_TEXTURE_2D, attachment->getID(), 0);

	_textureAttachments.push_back(attachment);

	return *this;
}
