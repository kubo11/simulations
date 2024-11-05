#ifndef SIMULATIONS_COMMON_APP
#define SIMULATIONS_COMMON_APP

#include "pch.hh"

#include "camera.hh"
#include "window.hh"

class App {
 public:
  App(const std::string& name);
  virtual ~App() = default;

  void run();

  static void close_callback(GLFWwindow* window);
  static void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
  static void mouse_position_callback(GLFWwindow* window, double xpos, double ypos);
  static void scroll_callback(GLFWwindow* window, double, double yoffset);

 protected:
  bool m_run = true;
  std::unique_ptr<Window> m_window;
  std::unique_ptr<Camera> m_camera;

  virtual void update(float dt) = 0;

 private:
  std::chrono::high_resolution_clock::time_point m_reference_time;
};

#endif  // SIMULATIONS_COMMON_APP