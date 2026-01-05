#ifndef SIMULATIONS_COMMON_FRAMEBUFFER
#define SIMULATIONS_COMMON_FRAMEBUFFER

#include "pch.hh"

class Framebuffer {
 public:
  Framebuffer(unsigned int width, unsigned int height);
  ~Framebuffer();

  PREVENT_COPY(Framebuffer);

  void resize(unsigned int width, unsigned int height);
  void bind();
  void unbind();

  GLuint get_texture_id() const;
  float get_aspect_ratio() const;

 private:
  unsigned int m_width;
  unsigned int m_height;
  GLuint m_fbo;
  GLuint m_rbo;
  GLuint m_texture_id;
  GLint m_prev_viewport[4];

  void create();
  void destroy();
};

#endif  // SIMULATIONS_COMMON_FRAMEBUFFER