#include "gl_texture.h"

#include <Windows.h>
#include <GL/GL.h>
#include <gl/GLU.h>
#include <iostream>

gl_texture::gl_texture() :
    m_id(0)
{
}

void gl_texture::create()
{
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}

void gl_texture::update_data(uint8_t* data)
{
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
}

void gl_texture::bind()
{
    glBindTexture(GL_TEXTURE_2D, m_id);
}

void gl_texture::unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}
