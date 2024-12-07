#ifndef SIMULATIONS_COMMON_TEXTURE
#define SIMULATIONS_COMMON_TEXTURE

#include "pch.hh"

enum class TextureWrapping { Repeat, MirroredRepeat, ClampToEdge, ClampToBorder };
enum class TextureFiltering { Nearest, Linear, NearestMipmapNearest, LinearMipmapNearest, NearestMipmapLinear, LinearMipmapLinear };

struct TextureConfig {
  TextureWrapping wrapping_u = TextureWrapping::Repeat;
  TextureWrapping wrapping_v = TextureWrapping::Repeat;
  TextureFiltering min_filtering = TextureFiltering::Linear;
  TextureFiltering mag_filtering = TextureFiltering::Linear;
  bool generate_mipmaps = false;
};

class Texture {
 public:
  static std::unique_ptr<Texture> load(const fs::path& path, const TextureConfig& config);

  virtual ~Texture();

  void bind(unsigned int unit = 0);
  void unbind(unsigned int unit = 0);

  unsigned int get_width() const;
  unsigned int get_heioght() const;
 private:
  GLuint m_id;
  unsigned int m_width;
  unsigned int m_height;

  Texture(unsigned char* data, unsigned int width, unsigned int height, unsigned int channels, const TextureConfig& config);

  static GLenum texture_wrapping_to_gl(TextureWrapping wrapping);
  static GLenum texture_filtering_to_gl(TextureFiltering filtering);
};

#endif // SIMULATIONS_COMMON_TEXTURE