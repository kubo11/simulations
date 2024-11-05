#ifndef SIMULATIONS_WHIRLIGIG_UI
#define SIMULATIONS_WHIRLIGIG_UI

#include "framebuffer.hh"
#include "pch.hh"
#include "ui.hh"
#include "whirligig.hh"
#include "window.hh"

class WhirligigUI : public UI {
 public:
  WhirligigUI(Window& window, Whirligig& whirligig, std::function<void(void)> start_handler,
              std::function<void(void)> stop_handler, std::function<void(void)> apply_handler,
              std::function<void(void)> skip_handler);

  void update_whirligig_data();
  void update_whirligig_parameters();
  void reset_whirligig_parameters();

  float get_dt();
  unsigned int get_skip_frames_count();
  float get_incline();
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
  float m_incline = 10.0f;
  float m_angular_velocity = 1.0f;
  unsigned int m_trajectory_length = 10000u;
  std::vector<std::unique_ptr<Function>> m_gravity_function;
  unsigned int m_selected_gravity_func_idx = 0;
  float m_dt = 0.01f;
  int m_skip_frames = 100;
  int m_path_length = 5000;

  glm::vec3 m_orientation = glm::vec3(0.0f, 0.0f, 0.0f);
  float m_gravity = 0.0f;
  glm::vec3 m_mass_center = glm::vec3(0.0f, 0.0f, 0.0f);

  std::mutex m_ui_mtx;
  Whirligig& m_whirligig;

  Window& m_window;

  bool m_start_button_enabled = false;
  bool m_stop_button_enabled = false;
  bool m_apply_button_enabled = false;
  bool m_skip_button_enabled = false;

  const std::function<void(void)> m_start_handler;
  const std::function<void(void)> m_stop_handler;
  const std::function<void(void)> m_apply_handler;
  const std::function<void(void)> m_skip_handler;

  void show_property_panel();
};

#endif  // SIMULATIONS_WHIRLIGIG_UI