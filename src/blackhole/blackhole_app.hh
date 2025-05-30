#ifndef SIMULATIONS_BLACKHOLE_APP
#define SIMULATIONS_BLACKHOLE_APP

#include "pch.hh"

#include "app.hh"
#include "blackhole_ui.hh"
#include "camera.hh"
#include "cubemap_texture.hh"
#include "message_queue.hh"
#include "shader_program.hh"
#include "vertex_array.hh"
#include "vertices.hh"

class BlackHoleApp : public App {
 public:
  BlackHoleApp();
  virtual ~BlackHoleApp() = default;

 protected:
  virtual void update(float dt) override;

 private:
  std::unique_ptr<BlackHoleUI> m_ui;
  std::unique_ptr<MessageQueueReader<BlackHoleMessage>> m_message_queue;
  std::unique_ptr<ShaderProgram> m_shader;
  std::unique_ptr<VertexArray<UVVertex>> m_fullscreen_triangle_vertex_array;
  std::unique_ptr<CubemapTexture> m_skybox;

  std::mutex m_visualization_mtx;

  void render_visualization();
  void handle_message(BlackHoleMessage msg);
};

#endif  // SIMULATIONS_BLACKHOLE_APP