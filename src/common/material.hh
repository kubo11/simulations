#ifndef SIMULATIONS_COMMON_MATERIAL
#define SIMULATIONS_COMMON_MATERIAL

#include "pch.hh"

#include "shader_program.hh"
#include "texture.hh"

using Color = glm::vec4;

struct Colors {
  static constexpr Color Black = {0.0f, 0.0f, 0.0f, 1.0f}; 
  static constexpr Color White = {1.0f, 1.0f, 1.0f, 1.0f};
  static constexpr Color Red = {1.0f, 0.0f, 0.0f, 1.0f};
  static constexpr Color Green = {0.0f, 1.0f, 0.0f, 1.0f};
  static constexpr Color Blue = {0.0f, 0.0f, 1.0f, 1.0f}; 
};

class Material {
 public:
  Material(Color color = Colors::White, std::unique_ptr<Texture> normal = nullptr);
  Material(std::unique_ptr<Texture> diffuse, std::unique_ptr<Texture> normal = nullptr);

  virtual ~Material() = default;

  void bind(ShaderProgram& shader);
  void unbind();
  
 private:
  Color m_color = Colors::White;
  std::unique_ptr<Texture> m_diffuse = nullptr;
  std::unique_ptr<Texture> m_normal = nullptr;
};

#endif // SIMULATIONS_COMMON_MATERIAL