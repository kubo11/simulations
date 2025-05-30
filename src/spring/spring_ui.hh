#ifndef SIMULATIONS_SPRING_UI
#define SIMULATIONS_SPRING_UI

#include "pch.hh"

#include "framebuffer.hh"
#include "function.hh"
#include "message_queue.hh"
#include "spring.hh"
#include "ui.hh"
#include "window.hh"

enum SpringMessage {
  Start,
  Stop,
  Restart,
  Apply,
  Skip
};

class SpringUI : public UI {
 public:
  SpringUI(Window& window, const Framebuffer& framebuffer, std::shared_ptr<MessageQueueWriter<SpringMessage>> message_queue);

  void update_spring_data(const Spring& spring);
  void update_spring_parameters(Spring& spring);
  void reset_spring(Spring& spring);
  void clear();

  float get_dt();
  unsigned int get_skip_frames_count();

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

  std::shared_ptr<MessageQueueWriter<SpringMessage>> m_message_queue;

  bool m_start_button_enabled = false;
  bool m_stop_button_enabled = false;
  bool m_apply_button_enabled = false;
  bool m_skip_button_enabled = false;

  void show_property_panel();
  void show_pos_vel_acc_graph();
  void show_forces_graph();
  void show_trajectory_graph();
  void show_visulaization();
};

#endif  // SIMULATIONS_SPRING_UI