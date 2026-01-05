#include "framebuffer.hh"

Framebuffer::Framebuffer(unsigned int width, unsigned int height) : m_width(width), m_height(height) {
  create();
}

Framebuffer::~Framebuffer() {
  destroy();
}

void Framebuffer::resize(unsigned int width, unsigned int height) {
  m_width = width;
  m_height = height;

  destroy();
  create();
}

void Framebuffer::bind() {
  glGetIntegerv(GL_VIEWPORT, m_prev_viewport);
  glViewport(0, 0, m_width, m_height);
  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
  glCheckError();
}

void Framebuffer::unbind() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(m_prev_viewport[0], m_prev_viewport[1], m_prev_viewport[2], m_prev_viewport[3]);
  glCheckError();
}

GLuint Framebuffer::get_texture_id() const { return m_texture_id; }

float Framebuffer::get_aspect_ratio() const {
  return static_cast<float>(m_width) / static_cast<float>(m_height);
}

void Framebuffer::create() {
  glGenFramebuffers(1, &m_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

  glGenTextures(1, &m_texture_id);
  glBindTexture(GL_TEXTURE_2D, m_texture_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture_id, 0);

  glGenRenderbuffers(1, &m_rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  glCheckError();
}

void Framebuffer::destroy() {
  glDeleteRenderbuffers(1, &m_rbo);
  glDeleteTextures(1, &m_texture_id);
  glDeleteFramebuffers(1, &m_fbo);
  glCheckError();
}