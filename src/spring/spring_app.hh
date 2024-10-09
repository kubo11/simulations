#ifndef SIMULATIONS_SPRING_APP
#define SIMULATIONS_SPRING_APP

#include "pch.hh"

#include "app.hh"
#include "framebuffer.hh"
#include "spring.hh"
#include "spring_ui.hh"
#include "window.hh"

class SpringApp : public App {
 public:
  SpringApp();
  ~SpringApp();

  static void close_callback(GLFWwindow* window);
  static void framebuffer_resize_callback(GLFWwindow* window, int width, int height);

  void simulation_loop();
  void start_simulatiton();
  void stop_simulation();
  void restart_simulation();

 protected:
  virtual void update(float dt) override;

 private:
  std::unique_ptr<Window> m_window;
  std::unique_ptr<Framebuffer> m_framebuffer;
  std::unique_ptr<Spring> m_spring;
  std::unique_ptr<SpringUI> m_ui;
  std::thread m_simulation_thread;
  std::mutex m_simulation_mtx;
  bool m_run_simulation = false;
  float m_dt;

  void copy_ui_data();
};

#endif  // SIMULATIONS_SPRING_APP