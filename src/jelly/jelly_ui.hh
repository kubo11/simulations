#ifndef SIMULATIONS_JELLY_UI
#define SIMULATIONS_JELLY_UI

#include "pch.hh"

#include "framebuffer.hh"
#include "message_queue.hh"
#include "ui.hh"
#include "jelly.hh"
#include "window.hh"

enum JellyMessage { Start, Stop, Restart, Apply, Skip, Distort, UpdateFrame, UpdateTexture, UpdateModel };

class JellyUI : public UI {
 public:
  JellyUI(Window& window, std::shared_ptr<MessageQueueWriter<JellyMessage>> message_queue);

  void update_jelly_parameters(Jelly& jelly);

  float get_dt();
  unsigned int get_skip_frames_count();

  glm::vec3 get_frame_position() const;
  glm::vec3 get_frame_orientation() const;
  bool get_control_springs_state() const;

  fs::path get_texture_path() const;
  fs::path get_model_path() const;

  bool show_control_points() const;
  bool show_control_point_springs() const;
  bool show_control_frame() const;
  bool show_control_frame_springs() const;
  bool show_bounding_box() const;
  bool show_bezier_cube() const;
  bool show_model() const;

 protected:
  virtual void draw() override;

 private:
  float m_dt = 0.001f;
  int m_skip_frames = 100;

  std::mutex m_ui_mtx;

  Window& m_window;

  bool m_show_control_points = true;
  bool m_show_control_point_springs = true;
  bool m_show_control_frame = true;
  bool m_show_control_frame_springs = false;
  bool m_show_bounding_box = true;
  bool m_show_bezier_cube = false;
  bool m_show_model = false;

  bool m_enable_show_model = false;

  float m_control_frame_position[3] = {};
  float m_control_frame_orientation[3] = {};

  float m_mass = 0.1f;
  float m_c1 = 10.0f;
  float m_c2 = 1.0f;
  float m_k = 0.1f;
  float m_distortion_amount = 0.4f;

  float m_gravity[3] = {};

  float m_collision_elasticity_coef = 1.0f;
  CollisionModel m_collision_model = CollisionModel::FullVelocityDamping;

  bool m_enable_control_frame_springs = true;

  std::string m_texture_path = fs::absolute(fs::path("resources/textures/slime.jpg")).string();
  std::string m_model_path = "";

  bool m_start_button_enabled = false;
  bool m_stop_button_enabled = false;
  bool m_apply_button_enabled = false;
  bool m_skip_button_enabled = false;

  std::shared_ptr<MessageQueueWriter<JellyMessage>> m_message_queue;

  void show_property_panel();
};

#endif  // SIMULATIONS_JELLY_UI