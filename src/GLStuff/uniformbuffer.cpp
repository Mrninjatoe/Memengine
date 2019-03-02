#include "GLStuff/uniformbuffer.hpp"

Uniformbuffer::Uniformbuffer(){
	glGenBuffers(1, &_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
}

Uniformbuffer::~Uniformbuffer(){

}
