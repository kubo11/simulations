#ifndef SIMULATIONS_SPRING_APP
#define SIMULATIONS_SPRING_APP

#include "pch.hh"

#include "app.hh"
#include "window.hh"

class SpringApp : public App {
 public:
  SpringApp();
  ~SpringApp();

  static void close_callback(GLFWwindow* window);
  static void framebuffer_resize_callback(GLFWwindow* window, int width, int height);

 protected:
  virtual void update(float dt) override;

 private:
  std::unique_ptr<Window> m_window;
};

#endif  // SIMULATIONS_SPRING_APP