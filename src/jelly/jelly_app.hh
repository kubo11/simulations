#ifndef SIMULATIONS_JELLY_APP
#define SIMULATIONS_JELLY_APP

#include "pch.hh"

#include "app.hh"
#include "buffer.hh"
#include "camera.hh"
#include "controllable_box.hh"
#include "framebuffer.hh"
#include "message_queue.hh"
#include "model.hh"
#include "shader_program.hh"
#include "simulation.hh"
#include "texture.hh"
#include "vertex_array.hh"
#include "jelly.hh"
#include "jelly_ui.hh"
#include "vertices.hh"
#include "window.hh"

class JellyApp : public App {
 public:
  JellyApp();
  virtual ~JellyApp();

 protected:
  virtual void update(float dt) override;

 private:
  std::unique_ptr<MessageQueueReader<JellyMessage>> m_message_queue;

  std::unique_ptr<Simulation<Jelly>> m_simulation;
  std::unique_ptr<JellyUI> m_ui;

  std::unique_ptr<VertexArray<Vertex>> m_control_points_vertex_array;
  std::unique_ptr<VertexArray<Vertex>> m_control_frame_springs_vertex_array;
  std::unique_ptr<VertexArray<Vertex>> m_bezier_cube_vertex_array;

  std::unique_ptr<ShaderProgram> m_basic_shader;
  std::unique_ptr<ShaderProgram> m_phong_shader;
  std::unique_ptr<ShaderProgram> m_bezier_cube_shader;
  std::unique_ptr<ShaderProgram> m_model_shader;
  std::unique_ptr<ShaderProgram> m_wireframe_shader;

  std::unique_ptr<Texture> m_bezier_cube_texture;
  std::unique_ptr<Model> m_model = nullptr;
  std::unique_ptr<ControllableBox> m_bounding_box;
  std::unique_ptr<ControllableBox> m_control_frame;

  std::vector<Vertex> m_control_points_vertices_staging = {};
  std::vector<Vertex> m_control_frame_springs_vertices_staging = {};

  std::array<glm::vec3, 64> m_points_vertices = {};

  std::mutex m_visualization_mtx;

  void render_visualization();
  void update_visualization_data(const Jelly& jelly);
  void handle_message(JellyMessage msg);
};

#endif  // SIMULATIONS_JELLY_APP