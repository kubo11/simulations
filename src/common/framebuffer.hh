#ifndef SIMULATIONS_COMMON_FRAMEBUFFER
#define SIMULATIONS_COMMON_FRAMEBUFFER

#include "pch.hh"

class Framebuffer {
 public:
  Framebuffer(unsigned int width, unsigned int height);
  ~Framebuffer();

  void resize(unsigned int width, unsigned int height);
  void bind();
  void unbind();

  GLuint get_texture_id() const;

 private:
  unsigned int m_width;
  unsigned int m_height;
  GLuint m_fbo;
  GLuint m_rbo;
  GLuint m_texture_id;
};

#endif  // SIMULATIONS_COMMON_FRAMEBUFFER