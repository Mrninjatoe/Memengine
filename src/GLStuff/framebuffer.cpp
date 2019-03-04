#include <stdio.h>
#include "engine.hpp"
#include "GLStuff/framebuffer.hpp"

Framebuffer::Framebuffer() : _name("No name"){
	glGenFramebuffers(1, &_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
}

Framebuffer::Framebuffer(const std::string& name) : _name(name) {
	glGenFramebuffers(1, &_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
}

Framebuffer::~Framebuffer() {
	glDeleteFramebuffers(1, &_fbo);
}

Framebuffer& Framebuffer::bind(const GLenum& io) {
	glBindFramebuffer(io, _fbo);

	return *this;
}

Framebuffer& Framebuffer::attachTexture(const unsigned int& pos, const glm::ivec2& size, 
	const Texture::TextureFormat& format, const bool& wantArray) {
	auto attachment = std::make_shared<Texture>(format, size, wantArray);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

	if (format == Texture::TextureFormat::Depth16f || format == Texture::TextureFormat::Depth24f ||
		format == Texture::TextureFormat::Depth32f) {
		if (!wantArray)
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, attachment->getID(), 0);
		else {
			//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_ARRAY, attachment->getID(), 0);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, attachment->getID(), 0);
		}
	}
	else {
		glFramebufferTexture2D(GL_FRAMEBUFFER, static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + pos), 
			GL_TEXTURE_2D, attachment->getID(), 0);
	}

	_textureAttachments.push_back(attachment);

	return *this;
}

void Framebuffer::finalize() {
	const GLenum buffers[]{ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
	if (_textureAttachments.size() > 1)
		glDrawBuffers(4, buffers);
	else {
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		printf("Successfully created framebuffer :sunglasses:\n");
	else
		printf("Unsuccessfully created a framebuffer :sad:\n");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
