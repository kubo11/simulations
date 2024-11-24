#ifndef SIMULATIONS_JELLY_UI
#define SIMULATIONS_JELLY_UI

#include "pch.hh"

#include "framebuffer.hh"
#include "message_queue.hh"
#include "ui.hh"
#include "jelly.hh"
#include "window.hh"

enum JellyMessage { Start, Stop, Restart, Apply, Skip, Distort, UpdateFrame };

class JellyUI : public UI {
 public:
  JellyUI(Window& window, std::shared_ptr<MessageQueueWriter<JellyMessage>> message_queue);

  void update_jelly_data(const Jelly& jelly);
  void update_jelly_parameters(Jelly& jelly);
  void reset_jelly(Jelly& jelly);

  float get_dt();
  unsigned int get_skip_frames_count();

  glm::vec3 get_frame_position() const;
  glm::vec3 get_frame_orientation() const;

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

  float m_control_frame_position[3] = {};
  float m_control_frame_orientation[3] = {};

  float m_mass = 1.0f;
  float m_c1 = 30.0f;
  float m_c2 = 30.0f;
  float m_k = 1.0f;
  float m_distortion_amount = 0.4f;

  bool m_enable_control_frame_springs = true;

  bool m_start_button_enabled = false;
  bool m_stop_button_enabled = false;
  bool m_apply_button_enabled = false;
  bool m_skip_button_enabled = false;

  std::shared_ptr<MessageQueueWriter<JellyMessage>> m_message_queue;

  void show_property_panel();
};

#endif  // SIMULATIONS_JELLY_UI