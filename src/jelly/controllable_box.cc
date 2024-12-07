#include "controllable_box.hh"

ControllableBox::ControllableBox(const glm::vec3& size, const glm::vec3& position, const glm::quat& orientation) : m_position(position), m_orientation(orientation), m_scale(2.0f * size) {
  update_model_mat();
  
  auto wireframe_vertices = std::vector<ColorVertex>{
    {{-0.5f, -0.5f, +0.5f}, {0.0f, 0.0f, 0.0f}}, {{+0.5f, -0.5f, +0.5f}, {0.0f, 0.0f, 0.0f}}, {{+0.5f, +0.5f, +0.5f}, {0.0f, 0.0f, 0.0f}}, {{-0.5f, +0.5f, +0.5f}, {0.0f, 0.0f, 0.0f}},
    {{+0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 0.0f}}, {{+0.5f, +0.5f, -0.5f}, {0.0f, 0.0f, 0.0f}}, {{-0.5f, +0.5f, -0.5f}, {0.0f, 0.0f, 0.0f}}, {{-0.5f, -0.5f, +0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}}, {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}}, {{+0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}}, {{-0.5f, +0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}}
  };
  auto wireframe_indices = std::vector<unsigned int>{0, 1, 1, 2, 2, 3, 3, 0, 7, 8, 9, 10, 5, 6, 5, 4, 1, 4, 11, 12, 2, 5, 3, 6};
  m_wireframe_vertex_array = VertexArray<ColorVertex>::create(wireframe_vertices, wireframe_indices, ColorVertex::get_vertex_attributes());
  
  auto solid_vertices = std::vector<NormalVertex>{
      {{-0.5f, -0.5f, +0.5f}, {+0.0f, +0.0f, -1.0f}}, {{+0.5f, +0.5f, +0.5f}, {+0.0f, +0.0f, -1.0f}}, {{+0.5f, -0.5f, +0.5f}, {+0.0f, +0.0f, -1.0f}},
      {{+0.5f, +0.5f, +0.5f}, {+0.0f, +0.0f, -1.0f}}, {{-0.5f, -0.5f, +0.5f}, {+0.0f, +0.0f, -1.0f}}, {{-0.5f, +0.5f, +0.5f}, {+0.0f, +0.0f, -1.0f}},
      {{+0.5f, -0.5f, +0.5f}, {-1.0f, +0.0f, +0.0f}}, {{+0.5f, +0.5f, -0.5f}, {-1.0f, +0.0f, +0.0f}}, {{+0.5f, -0.5f, -0.5f}, {-1.0f, +0.0f, +0.0f}},
      {{+0.5f, +0.5f, -0.5f}, {-1.0f, +0.0f, +0.0f}}, {{+0.5f, -0.5f, +0.5f}, {-1.0f, +0.0f, +0.0f}}, {{+0.5f, +0.5f, +0.5f}, {-1.0f, +0.0f, +0.0f}},
      {{+0.5f, -0.5f, -0.5f}, {+0.0f, +0.0f, +1.0f}}, {{-0.5f, +0.5f, -0.5f}, {+0.0f, +0.0f, +1.0f}}, {{-0.5f, -0.5f, -0.5f}, {+0.0f, +0.0f, +1.0f}},
      {{-0.5f, +0.5f, -0.5f}, {+0.0f, +0.0f, +1.0f}}, {{+0.5f, -0.5f, -0.5f}, {+0.0f, +0.0f, +1.0f}}, {{+0.5f, +0.5f, -0.5f}, {+0.0f, +0.0f, +1.0f}},
      {{-0.5f, -0.5f, -0.5f}, {+1.0f, +0.0f, +0.0f}}, {{-0.5f, +0.5f, +0.5f}, {+1.0f, +0.0f, +0.0f}}, {{-0.5f, -0.5f, +0.5f}, {+1.0f, +0.0f, +0.0f}},
      {{-0.5f, +0.5f, +0.5f}, {+1.0f, +0.0f, +0.0f}}, {{-0.5f, -0.5f, -0.5f}, {+1.0f, +0.0f, +0.0f}}, {{-0.5f, +0.5f, -0.5f}, {+1.0f, +0.0f, +0.0f}},
      {{-0.5f, +0.5f, +0.5f}, {+0.0f, -1.0f, +0.0f}}, {{+0.5f, +0.5f, -0.5f}, {+0.0f, -1.0f, +0.0f}}, {{+0.5f, +0.5f, +0.5f}, {+0.0f, -1.0f, +0.0f}},
      {{+0.5f, +0.5f, -0.5f}, {+0.0f, -1.0f, +0.0f}}, {{-0.5f, +0.5f, +0.5f}, {+0.0f, -1.0f, +0.0f}}, {{-0.5f, +0.5f, -0.5f}, {+0.0f, -1.0f, +0.0f}},
      {{-0.5f, -0.5f, -0.5f}, {+0.0f, +1.0f, +0.0f}}, {{+0.5f, -0.5f, +0.5f}, {+0.0f, +1.0f, +0.0f}}, {{+0.5f, -0.5f, -0.5f}, {+0.0f, +1.0f, +0.0f}},
      {{+0.5f, -0.5f, +0.5f}, {+0.0f, +1.0f, +0.0f}}, {{-0.5f, -0.5f, -0.5f}, {+0.0f, +1.0f, +0.0f}}, {{-0.5f, -0.5f, +0.5f}, {+0.0f, +1.0f, +0.0f}},
  };

  m_solid_vertex_array = VertexArray<NormalVertex>::create(solid_vertices, NormalVertex::get_vertex_attributes());
}

const glm::vec3& ControllableBox::get_position() const {
  return m_position;
}

const glm::quat& ControllableBox::get_orientation() const {
  return m_orientation;
}

const glm::vec3& ControllableBox::get_scale() const {
  return m_scale;
}

unsigned int ControllableBox::get_wireframe_width() const {
  return m_wireframe_width;
}

const glm::mat4& ControllableBox::get_model_mat() const {
  return m_model_mat;
}

const glm::vec4& ControllableBox::get_solid_color() const {
  return m_solid_color;
}

void ControllableBox::set_position(const glm::vec3& position) {
  m_position = position;
  update_model_mat();
}

void ControllableBox::set_orientation(const glm::quat& orientation) {
  m_orientation = orientation;
  update_model_mat();
}

void ControllableBox::set_scale(const glm::vec3& scale) {
  m_scale = scale;
  update_model_mat();
}

void ControllableBox::set_wireframe_width(unsigned int width) {
  m_wireframe_width = width;
}

void ControllableBox::set_solid_color(const glm::vec4& color) {
  m_solid_color = color;
}

// std::optional<CollisionData> ControllableBox::check_collisions(const glm::vec3& origin, const glm::vec3& ray) const {

// }

void ControllableBox::draw_solid(ShaderProgram& shader) {
  shader.set_uniform_value("model", m_model_mat);
  shader.set_uniform_value("color", m_solid_color);
  shader.bind();
  m_solid_vertex_array->bind();
  glDrawArrays(GL_TRIANGLES, 0, m_solid_vertex_array->get_draw_size());
  m_solid_vertex_array->unbind();
  shader.unbind();
}

void ControllableBox::draw_wireframe(ShaderProgram& shader) {
  glLineWidth(m_wireframe_width);
  shader.set_uniform_value("model", m_model_mat);
  shader.bind();
  m_wireframe_vertex_array->bind();
  glDrawElements(GL_LINES, m_wireframe_vertex_array->get_draw_size(), GL_UNSIGNED_INT, 0);
  m_wireframe_vertex_array->unbind();
  shader.unbind();
}

void ControllableBox::update_model_mat() {
  m_model_mat = glm::translate(glm::scale(m_scale), m_position) * glm::mat4(m_orientation);
}