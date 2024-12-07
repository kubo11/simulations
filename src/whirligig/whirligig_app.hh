#ifndef SIMULATIONS_WHIRLIGIG_APP
#define SIMULATIONS_WHIRLIGIG_APP

#include "pch.hh"

#include "app.hh"
#include "buffer.hh"
#include "camera.hh"
#include "framebuffer.hh"
#include "message_queue.hh"
#include "shader_program.hh"
#include "simulation.hh"
#include "vertex_array.hh"
#include "vertices.hh"
#include "whirligig.hh"
#include "whirligig_ui.hh"
#include "window.hh"

class WhirligigApp : public App {
 public:
  WhirligigApp();
  virtual ~WhirligigApp();

 protected:
  virtual void update(float dt) override;

 private:
  std::unique_ptr<MessageQueueReader<WhirligigMessage>> m_message_queue;

  std::unique_ptr<Simulation<Whirligig>> m_simulation;
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
  void update_visualization_data(const Whirligig& whirligig);
  void handle_message(WhirligigMessage msg);
};

#endif  // SIMULATIONS_WHIRLIGIG_APP