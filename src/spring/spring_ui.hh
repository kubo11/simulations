#ifndef SIMULATIONS_SPRING_UI
#define SIMULATIONS_SPRING_UI

#include "framebuffer.hh"
#include "function.hh"
#include "pch.hh"
#include "ui.hh"
#include "window.hh"

class SpringUI : public UI {
 public:
  SpringUI(const Window& window, const Framebuffer& framebuffer, std::function<void(void)> start_handler,
           std::function<void(void)> pause_handler, std::function<void(void)> restart_handler, std::function<void(void)> skip_handler);

  float get_weight_starting_position();
  float get_weight_starting_velocity();
  float get_dt();
  float get_weight_mass();
  float get_elasticity_coef();
  float get_damping_coef();
  unsigned int get_frames_to_skip();
  std::unique_ptr<Function> get_rest_position_function();
  std::unique_ptr<Function> get_field_force_function();

  void update_spring_data(float position, float velocity, float acceleration, float elasticity, float damping,
                          float field, float rest_position, float time);
  void clear();

 protected:
  virtual void draw() override;

 private:
  float m_weight_starting_position = -2.0f;
  float m_weight_starting_velocity = 0.0f;
  float m_dt = 0.01f;
  float m_weight_mass = 1.0f;
  float m_elasticity_coef = 5.0f;
  float m_damping_coef = 0.1f;
  std::vector<std::unique_ptr<Function>> m_rest_position_functions;
  unsigned int m_selected_rest_pos_func_idx = 0;
  std::vector<std::unique_ptr<Function>> m_field_force_function;
  unsigned int m_selected_field_force_func_idx = 0;
  int m_skip_frames = 100;

  std::mutex m_ui_mtx;

  std::vector<float> m_time;
  std::vector<float> m_weight_position;
  std::vector<float> m_weight_velocity;
  std::vector<float> m_weight_acceleration;
  std::vector<float> m_elasticity_force;
  std::vector<float> m_damping_force;
  std::vector<float> m_field_force;
  std::vector<float> m_rest_position;

  const Window& m_window;
  const Framebuffer& m_framebuffer;

  const std::function<void(void)> m_start_handler;
  const std::function<void(void)> m_pause_handler;
  const std::function<void(void)> m_restart_handler;
  const std::function<void(void)> m_skip_handler;

  void show_property_panel();
  void show_pos_vel_acc_graph();
  void show_forces_graph();
  void show_trajectory_graph();
  void show_visulaization();
};

#endif  // SIMULATIONS_SPRING_UI