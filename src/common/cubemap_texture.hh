#ifndef SIMULATIONS_COMMON_CUBEMAP_TEXTURE
#define SIMULATIONS_COMMON_CUBEMAP_TEXTURE

#include "pch.hh"

#include "texture.hh"
#include "stb_image.h"

struct CubemapTextureConfig {
  TextureWrapping wrapping_u = TextureWrapping::Repeat;
  TextureWrapping wrapping_v = TextureWrapping::Repeat;
  TextureWrapping wrapping_w = TextureWrapping::Repeat;
  TextureFiltering min_filtering = TextureFiltering::Linear;
  TextureFiltering mag_filtering = TextureFiltering::Linear;
  bool generate_mipmaps = false;
};

class CubemapTexture {
 public:
  static std::unique_ptr<CubemapTexture> load(const std::array<fs::path, 6>& paths, const CubemapTextureConfig& config);

  virtual ~CubemapTexture();

  void bind(unsigned int unit = 0);
  void unbind(unsigned int unit = 0);

  unsigned int get_width() const;
  unsigned int get_heioght() const;
 private:
  GLuint m_id;
  unsigned int m_width;
  unsigned int m_height;

  CubemapTexture(const std::array<unsigned char*, 6>& data, unsigned int width, unsigned int height, unsigned int channels, const CubemapTextureConfig& config);
};

#endif // SIMULATIONS_COMMON_CUBEMAP_TEXTURE