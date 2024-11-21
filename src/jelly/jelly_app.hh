#ifndef SIMULATIONS_JELLY_APP
#define SIMULATIONS_JELLY_APP

#include "pch.hh"

#include "app.hh"
#include "buffer.hh"
#include "camera.hh"
#include "framebuffer.hh"
#include "message_queue.hh"
#include "shader_program.hh"
#include "simulation.hh"
#include "vertex_array.hh"
#include "jelly.hh"
#include "jelly_ui.hh"
#include "window.hh"

struct Vertex {
  glm::vec3 position;

  Vertex() : position{0.0f, 0.0f, 0.0f} {}
  Vertex(const glm::vec3& position) : position(position) {}

  static std::vector<VertexAttribute> get_vertex_attributes() { return {{.size = 3, .type = GL_FLOAT}}; }
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

  std::mutex m_visualization_mtx;

  void render_visualization();
  void update_visualization_data(const Jelly& jelly);
  void handle_message(JellyMessage msg);
};

#endif  // SIMULATIONS_JELLY_APP