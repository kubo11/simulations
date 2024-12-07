#include "texture.hh"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(unsigned char* data, unsigned int width, unsigned int height, unsigned int channels, const TextureConfig& config) {
  glGenTextures(1, &m_id);
  glCheckError();
  glBindTexture(GL_TEXTURE_2D, m_id);
  glCheckError();
  auto mode = (channels == 3) ? GL_RGB : GL_RGBA;
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, mode, GL_UNSIGNED_BYTE, data);
  glCheckError();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture_wrapping_to_gl(config.wrapping_u));
  glCheckError();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture_wrapping_to_gl(config.wrapping_v));
  glCheckError();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture_filtering_to_gl(config.min_filtering));
  glCheckError();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture_filtering_to_gl(config.mag_filtering));
  glCheckError();
  if (config.generate_mipmaps) {
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  glCheckError();
}

Texture::~Texture() {
  if (m_id) {
    glDeleteTextures(1, &m_id);
    glCheckError();
    m_id = 0;
  }
}

std::unique_ptr<Texture> Texture::load(const fs::path& path, const TextureConfig& config) {
  int width, height, channels;
  stbi_set_flip_vertically_on_load(true); 
  unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 0);
  auto texture = std::unique_ptr<Texture>(new Texture(data, width, height, channels, config));
  stbi_image_free(data);
  return texture;
}

void Texture::bind(unsigned int unit) {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D, m_id);
  glCheckError();
}

void Texture::unbind(unsigned int unit) {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D, 0);
  glCheckError();
}

unsigned int Texture::get_width() const {
  return m_width;
}

unsigned int Texture::get_heioght() const {
  return m_height;
}

GLenum Texture::texture_wrapping_to_gl(TextureWrapping wrapping) {
  switch (wrapping) {
    case TextureWrapping::Repeat:
      return GL_REPEAT;
    case TextureWrapping::MirroredRepeat:
      return GL_MIRRORED_REPEAT;
    case TextureWrapping::ClampToEdge:
      return GL_CLAMP_TO_EDGE;
    case TextureWrapping::ClampToBorder:
      return GL_CLAMP_TO_BORDER;
    
    default:
      return GL_NONE;
  }
}

GLenum Texture::texture_filtering_to_gl(TextureFiltering filtering) {
  switch (filtering) {
    case TextureFiltering::Linear:
      return GL_LINEAR;
    case TextureFiltering::Nearest:
      return GL_NEAREST;
    case TextureFiltering::LinearMipmapLinear:
      return GL_LINEAR_MIPMAP_LINEAR;
    case TextureFiltering::LinearMipmapNearest:
      return GL_LINEAR_MIPMAP_NEAREST;
    case TextureFiltering::NearestMipmapLinear:
      return GL_LINEAR_MIPMAP_LINEAR;
    case TextureFiltering::NearestMipmapNearest:
      return GL_LINEAR_MIPMAP_NEAREST;
    
    default:
      return GL_NONE;
  }
}