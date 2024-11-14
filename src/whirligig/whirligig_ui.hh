#ifndef SIMULATIONS_WHIRLIGIG_UI
#define SIMULATIONS_WHIRLIGIG_UI

#include "pch.hh"

#include "framebuffer.hh"
#include "message_queue.hh"
#include "ui.hh"
#include "whirligig.hh"
#include "window.hh"

enum WhirligigMessage {
  Start,
  Stop,
  Restart,
  Apply,
  Skip
};

class WhirligigUI : public UI {
 public:
  WhirligigUI(Window& window, std::shared_ptr<MessageQueueWriter<WhirligigMessage>> message_queue);

  void update_whirligig_data(const Whirligig& whirligig);
  void update_whirligig_parameters(Whirligig& whirligig);
  void reset_whirligig(Whirligig& whirligig);

  float get_dt();
  unsigned int get_skip_frames_count();
  float get_cube_tilt();
  unsigned int get_trajectory_length();

  bool show_cube();
  bool show_diagonal();
  bool show_trajectory();
  bool show_gravity_vector();
  bool show_plane();

 protected:
  virtual void draw() override;

 private:
  bool m_show_cube = true;
  bool m_show_diagonal = true;
  bool m_show_trajectory = true;
  bool m_show_gravity_vector = true;
  bool m_show_plane = true;
  bool m_enable_gravity = true;

  float m_cube_size = 1.0f;
  float m_cube_density = 1.0f;
  float m_cube_tilt = 15.0f;
  float m_starting_angular_velocity = 45.0f;
  std::vector<std::unique_ptr<Function>> m_gravity_function;
  unsigned int m_selected_gravity_func_idx = 0;
  float m_dt = 0.001f;
  int m_skip_frames = 100;
  int m_path_length = 5000000;

  glm::vec3 m_orientation = glm::vec3(0.0f, 0.0f, 0.0f);
  float m_gravity = 0.0f;
  glm::vec3 m_mass_center = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 m_angular_velocity = glm::vec3(0.0f, 0.0f, 0.0f);

  std::mutex m_ui_mtx;

  Window& m_window;

  bool m_start_button_enabled = false;
  bool m_stop_button_enabled = false;
  bool m_apply_button_enabled = false;
  bool m_skip_button_enabled = false;

  std::shared_ptr<MessageQueueWriter<WhirligigMessage>> m_message_queue;

  void show_property_panel();
};

#endif  // SIMULATIONS_WHIRLIGIG_UI