#ifndef SIMULATIONS_SPRING_APP
#define SIMULATIONS_SPRING_APP

#include "pch.hh"

#include "app.hh"
#include "buffer.hh"
#include "camera.hh"
#include "framebuffer.hh"
#include "shader_program.hh"
#include "spring.hh"
#include "spring_ui.hh"
#include "vertex_array.hh"
#include "window.hh"

struct WeightVertex {
  glm::vec3 position;
  glm::vec3 color;

  WeightVertex() : position{0.0f, 0.0f, 0.0f}, color{0.0f, 0.0f, 0.0f} {}
  WeightVertex(const glm::vec3& position, const glm::vec3& color) : position(position), color(color) {}

  static std::vector<VertexAttribute> get_vertex_attributes() {
    return {{.size = 3, .type = GL_FLOAT}, {.size = 3, .type = GL_FLOAT}};
  }
};

class SpringApp : public App {
 public:
  SpringApp();
  ~SpringApp();

  static void close_callback(GLFWwindow* window);
  static void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
  static void mouse_position_callback(GLFWwindow* window, double xpos, double ypos);
  static void scroll_callback(GLFWwindow* window, double, double yoffset);

  void simulation_loop();
  void start_simulatiton();
  void stop_simulation();
  void restart_simulation();

 protected:
  virtual void update(float dt) override;

 private:
  std::unique_ptr<Window> m_window;
  std::unique_ptr<Framebuffer> m_framebuffer;
  std::unique_ptr<Spring> m_spring;
  std::unique_ptr<SpringUI> m_ui;
  std::thread m_simulation_thread;
  std::mutex m_simulation_mtx;
  bool m_run_simulation = false;
  float m_dt;

  std::unique_ptr<ShaderProgram> m_spring_shader;
  std::unique_ptr<ShaderProgram> m_weight_shader;
  std::unique_ptr<VertexArray<int>> m_spring_vertex_array;
  std::unique_ptr<VertexArray<WeightVertex>> m_weight_vertex_array;
  glm::mat4 m_model_mat = glm::mat4(1.0f);
  std::unique_ptr<Camera> m_camera;

  void copy_ui_data();
  void render_visualization();
};

#endif  // SIMULATIONS_SPRING_APP