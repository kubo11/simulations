#ifndef SIMULATIONS_FLYWHEEL_APP
#define SIMULATIONS_FLYWHEEL_APP

#include "pch.hh"

#include "app.hh"
#include "buffer.hh"
#include "camera.hh"
#include "framebuffer.hh"
#include "message_queue.hh"
#include "shader_program.hh"
#include "flywheel.hh"
#include "simulation.hh"
#include "flywheel_ui.hh"
#include "vertex_array.hh"
#include "window.hh"
#include "model.hh"

class FlywheelApp : public App {
 static constexpr glm::vec2 m_frame_size_multiplier = {0.5f, 0.7f};
 public:
  FlywheelApp();
  virtual ~FlywheelApp() override;

  static void framebuffer_resize_callback(GLFWwindow* window, int width, int height);

 protected:
  virtual void update(float dt) override;

 private:
  std::unique_ptr<MessageQueueReader<FlywheelMessage>> m_message_queue;

  std::unique_ptr<Framebuffer> m_framebuffer;
  std::unique_ptr<Simulation<Flywheel>> m_flywheel_simulation;
  std::unique_ptr<FlywheelUI> m_ui;

  std::unique_ptr<ShaderProgram> m_shader;
  std::unique_ptr<Model> m_flywheel_model;
  std::unique_ptr<Model> m_handle_model;
  std::unique_ptr<Model> m_piston_model;

  void render_visualization();
  void update_visualization_data(const Flywheel& flywheel);
  void handle_message(FlywheelMessage msg);
};

#endif  // SIMULATIONS_FLYWHEEL_APP