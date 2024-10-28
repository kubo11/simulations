#ifndef SIMULATIONS_WHIRLIGIG_APP
#define SIMULATIONS_WHIRLIGIG_APP

#include "pch.hh"

#include "app.hh"
#include "buffer.hh"
#include "camera.hh"
#include "framebuffer.hh"
#include "shader_program.hh"
#include "vertex_array.hh"
#include "whirligig.hh"
#include "whirligig_ui.hh"
#include "whirligig_simulation.hh"
#include "window.hh"

struct Vertex {
  glm::vec3 position;

  Vertex() : position{0.0f, 0.0f, 0.0f} {}
  Vertex(const glm::vec3& position) : position(position) {}

  static std::vector<VertexAttribute> get_vertex_attributes() {
    return {{.size = 3, .type = GL_FLOAT}};
  }
};

struct NormalVertex {
  glm::vec3 position;
  glm::vec3 normal;

  NormalVertex() : position{0.0f, 0.0f, 0.0f}, normal{0.0f, 0.0f, 0.0f} {}
  NormalVertex(const glm::vec3& position, const glm::vec3& normal) : position(position), normal(normal) {}

  static std::vector<VertexAttribute> get_vertex_attributes() {
    return {{.size = 3, .type = GL_FLOAT}, {.size = 3, .type = GL_FLOAT}};
  }
};

struct TextureVertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 tex;

  TextureVertex() : position{0.0f, 0.0f, 0.0f}, normal{0.0f, 0.0f, 0.0f}, tex{0.0f, 0.0f} {}
  TextureVertex(const glm::vec3& position, const glm::vec3& normal, glm::vec2& tex)
      : position(position), normal(normal), tex(tex) {}

  static std::vector<VertexAttribute> get_vertex_attributes() {
    return {{.size = 3, .type = GL_FLOAT}, {.size = 3, .type = GL_FLOAT}, {.size = 2, .type = GL_FLOAT}};
  }
};

class WhirligigApp : public App {
 public:
  WhirligigApp();
  virtual ~WhirligigApp();

 protected:
  virtual void update(float dt) override;

 private:
  std::unique_ptr<Whirligig> m_whirligig;
  std::unique_ptr<WhirligigSimulation> m_simulation;
  std::unique_ptr<WhirligigUI> m_ui;

  std::unique_ptr<VertexArray<NormalVertex>> m_plane_vertex_array;
  std::unique_ptr<VertexArray<NormalVertex>> m_cube_vertex_array;
  std::unique_ptr<VertexArray<Vertex>> m_diagonal_vertex_array;
  std::unique_ptr<VertexArray<Vertex>> m_gravity_vector_vertex_array;
  std::unique_ptr<VertexArray<Vertex>> m_trajectory_vertex_array;

  std::unique_ptr<ShaderProgram> m_basic_shader;
  std::unique_ptr<ShaderProgram> m_phong_shader;

  glm::mat4 m_plane_model_matrix = glm::mat4(1.0f);
  glm::mat4 m_cube_model_matrix = glm::mat4(1.0f);
  glm::mat4 m_gravity_vector_model_matrix = glm::mat4(1.0f);

  unsigned int m_trajectory_offset = 0u;
  unsigned int m_trajectory_length = 0u;
  std::vector<Vertex> m_trajectory_vertices;

  std::mutex m_visualization_mtx;

  void render_visualization();
  void update_visualization_data();
};

#endif  // SIMULATIONS_WHIRLIGIG_APP