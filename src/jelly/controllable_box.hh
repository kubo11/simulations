#ifndef SIMULATIONS_JELLY_CONTROLLABLE_BOX
#define SIMULATIONS_JELLY_CONTROLLABLE_BOX

#include "pch.hh"

#include "buffer.hh"
#include "shader_program.hh"
#include "vertex_array.hh"
#include "vertices.hh"

struct CollisionData {
  glm::vec3 contact;
  glm::vec3 normal;
};

class ControllableBox {
 public:
  ControllableBox(const glm::vec3& size = {0.5f, 0.5f, 0.5f}, const glm::vec3& position = {0.0f, 0.0f, 0.0f}, const glm::quat& orientation = {1.0f, 0.0f, 0.0f, 0.0f});
  virtual ~ControllableBox() = default;

  const glm::vec3& get_position() const;
  const glm::quat& get_orientation() const;
  const glm::vec3& get_scale() const;
  unsigned int get_wireframe_width() const;
  const glm::mat4& get_model_mat() const;
  const glm::vec4& get_solid_color() const;

  void set_position(const glm::vec3& position);
  void set_orientation(const glm::quat& orientation);
  void set_scale(const glm::vec3& scale);
  void set_wireframe_width(unsigned int width);
  void set_solid_color(const glm::vec4& color);

  // std::optional<CollisionData> check_collisions(const glm::vec3& origin, const glm::vec3& ray) const;

  void draw_solid(ShaderProgram& shader);
  void draw_wireframe(ShaderProgram& shader);

 private:
  unsigned int m_wireframe_width = 1;
  glm::vec3 m_position = {0.0f, 0.0f, 0.0f};
  glm::quat m_orientation = {1.0f, 0.0f, 0.0f, 0.0f};
  glm::vec3 m_scale = {1.0f, 1.0f, 1.0f};
  glm::vec4 m_solid_color = {1.0f, 1.0f, 1.0f, 1.0f};
  glm::mat4 m_model_mat = glm::mat4(1.0f);

  std::unique_ptr<VertexArray<NormalVertex>> m_solid_vertex_array = nullptr;
  std::unique_ptr<VertexArray<ColorVertex>> m_wireframe_vertex_array = nullptr;

  void update_model_mat();
};

#endif // SIMULATIONS_JELLY_CONTROLLABLE_BOX