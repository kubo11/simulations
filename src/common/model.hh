#ifndef SIMULATIONS_COMMON_MODEL
#define SIMULATIONS_COMMON_MODEL

#include "pch.hh"

#include "vertex_array.hh"
#include "material.hh"
#include "texture.hh"
#include "vertices.hh"

class Model {
 public:
  ~Model() = default;

  static std::unique_ptr<Model> load(const fs::path& path);

  const glm::mat4& get_model_mat() const;
  Material& get_material();
  void set_model_mat(const glm::mat4& model_mat);

  void draw(ShaderProgram& shader);

 private:
  std::unique_ptr<VertexArray<TextureVertex>> m_vertex_array = nullptr;
  std::unique_ptr<Material> m_material = nullptr;
  glm::mat4 m_model_mat = glm::mat4(1.0f);

  Model(std::unique_ptr<VertexArray<TextureVertex>> vertex_array, std::unique_ptr<Material> material);
};

#endif // SIMULATIONS_COMMON_MODEL