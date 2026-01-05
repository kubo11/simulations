#ifndef SIMULATIONS_FLYWHEEL_UI
#define SIMULATIONS_FLYWHEEL_UI

#include "pch.hh"

#include "framebuffer.hh"
#include "function.hh"
#include "message_queue.hh"
#include "flywheel.hh"
#include "ui.hh"
#include "window.hh"

enum FlywheelMessage {
  Start,
  Stop,
  Restart,
  Apply,
  Skip
};

class FlywheelUI : public UI {
 public:
  FlywheelUI(Window& window, const Framebuffer& framebuffer, std::shared_ptr<MessageQueueWriter<FlywheelMessage>> message_queue);

  void update_flywheel_data(const Flywheel& flywheel);
  void update_flywheel_parameters(Flywheel& flywheel);
  void reset_flywheel(Flywheel& flywheel);
  void clear();

  float get_dt();
  unsigned int get_skip_frames_count();

 protected:
  virtual void draw() override;

 private:
  float m_dt = 0.01f;
  float m_angular_velocity = 1.0f;
  float m_radius = 1.0f;
  float m_length = 2.0f;
  float m_standard_deviation = 0.0f;
  int m_skip_frames = 100;
  bool m_autofit_x = true;
  bool m_autofit_y = true;

  std::mutex m_ui_mtx;

  std::vector<float> m_time;
  std::vector<float> m_piston_position;
  std::vector<float> m_piston_velocity;
  std::vector<float> m_piston_acceleration;

  const Window& m_window;
  const Framebuffer& m_framebuffer;

  std::shared_ptr<MessageQueueWriter<FlywheelMessage>> m_message_queue;

  bool m_start_button_enabled = false;
  bool m_stop_button_enabled = false;
  bool m_apply_button_enabled = false;
  bool m_skip_button_enabled = false;

  void show_property_panel();
  void show_pos_graph();
  void show_vel_graph();
  void show_acc_graph();
  void show_trajectory_graph();
  void show_visulaization();
};

#endif  // SIMULATIONS_FLYWHEEL_UI