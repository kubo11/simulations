#ifndef SIMULATIONS_SPRING_APP
#define SIMULATIONS_SPRING_APP

#include "pch.hh"

#include "app.hh"
#include "buffer.hh"
#include "camera.hh"
#include "framebuffer.hh"
#include "shader_program.hh"
#include "spring.hh"
#include "spring_simulation.hh"
#include "spring_ui.hh"
#include "vertex_array.hh"
#include "window.hh"

struct WeightVertex {
  glm::vec3 position;
  glm::vec3 normal;

  WeightVertex() : position{0.0f, 0.0f, 0.0f}, normal{0.0f, 0.0f, 0.0f} {}
  WeightVertex(const glm::vec3& position, const glm::vec3& normal) : position(position), normal(normal) {}

  static std::vector<VertexAttribute> get_vertex_attributes() {
    return {{.size = 3, .type = GL_FLOAT}, {.size = 3, .type = GL_FLOAT}};
  }
};

class SpringApp : public App {
 public:
  SpringApp();
  virtual ~SpringApp() override;

  static void framebuffer_resize_callback(GLFWwindow* window, int width, int height);

 protected:
  virtual void update(float dt) override;

 private:
  std::unique_ptr<Framebuffer> m_framebuffer;
  std::unique_ptr<SpringSimulation> m_spring_simulation;
  std::unique_ptr<SpringUI> m_ui;
  std::unique_ptr<Spring> m_spring;
  float m_spring_height = 2.0f;

  std::unique_ptr<ShaderProgram> m_spring_shader;
  std::unique_ptr<ShaderProgram> m_weight_shader;
  std::unique_ptr<VertexArray<int>> m_spring_vertex_array;
  std::unique_ptr<VertexArray<WeightVertex>> m_weight_vertex_array;
  glm::mat4 m_weight_model_mat = glm::mat4(1.0f);
  glm::mat4 m_spring_model_mat = glm::mat4(1.0f);

  void render_visualization();
};

#endif  // SIMULATIONS_SPRING_APP