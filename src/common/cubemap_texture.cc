#include "cubemap_texture.hh"

CubemapTexture::CubemapTexture(const std::array<unsigned char*, 6>& data, unsigned int width, unsigned int height, unsigned int channels, const CubemapTextureConfig& config) {
  glGenTextures(1, &m_id);
  glCheckError();
  glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
  glCheckError();
  auto mode = (channels == 3) ? GL_RGB : GL_RGBA;
  for (const auto& [idx, dat] : std::views::enumerate(data) | std::views::as_const) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + idx, 0, GL_RGB, width, height, 0, mode, GL_UNSIGNED_BYTE, dat);
      glCheckError();
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, Texture::texture_wrapping_to_gl(config.wrapping_u));
  glCheckError();
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, Texture::texture_wrapping_to_gl(config.wrapping_v));
  glCheckError();
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, Texture::texture_wrapping_to_gl(config.wrapping_w));
  glCheckError();
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, Texture::texture_filtering_to_gl(config.min_filtering));
  glCheckError();
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, Texture::texture_filtering_to_gl(config.mag_filtering));
  glCheckError();
  if (config.generate_mipmaps) {
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
  }
  glCheckError();
}

CubemapTexture::~CubemapTexture() {
  if (m_id) {
    glDeleteTextures(1, &m_id);
    glCheckError();
    m_id = 0;
  }
}

std::unique_ptr<CubemapTexture> CubemapTexture::load(const std::array<fs::path, 6>& paths, const CubemapTextureConfig& config) {
  int width, height, channels;
  std::array<unsigned char*, 6> data; 
  for (const auto& [idx, path] : std::views::enumerate(paths) | std::views::as_const) {
    data[idx] = stbi_load(path.c_str(), &width, &height, &channels, 0);
  }
  auto cubemap_texture = std::unique_ptr<CubemapTexture>(new CubemapTexture(data, width, height, channels, config));
  for (const auto& dat : data) {
    stbi_image_free(dat);
  }
  return cubemap_texture;
}

void CubemapTexture::bind(unsigned int unit) {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
  glCheckError();
}

void CubemapTexture::unbind(unsigned int unit) {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  glCheckError();
}

unsigned int CubemapTexture::get_width() const {
  return m_width;
}

unsigned int CubemapTexture::get_heioght() const {
  return m_height;
}