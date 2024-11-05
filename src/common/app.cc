#include "app.hh"

App::App(const std::string& name) : m_reference_time(std::chrono::high_resolution_clock::now()) {
  m_window = std::make_unique<Window>(name, 1280u, 720u, glm::vec3(100.0f / 256.f, 100.0f / 256.0f, 100.0f / 256.0f));

  glfwSetWindowUserPointer(m_window->get_instance(), this);

  glfwSetWindowCloseCallback(m_window->get_instance(), App::close_callback);
  glfwSetFramebufferSizeCallback(m_window->get_instance(), App::framebuffer_resize_callback);
  glfwSetCursorPosCallback(m_window->get_instance(), App::mouse_position_callback);
  glfwSetScrollCallback(m_window->get_instance(), App::scroll_callback);

  auto res = gladLoadGL(glfwGetProcAddress);
  if (res == 0) {
    throw std::runtime_error("Failed to initialize GLAD");
  }

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glEnable(GL_MULTISAMPLE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  m_camera = std::make_unique<Camera>(glm::vec3{0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 8.0f, 1.6f, 64.0f, 45.0f,
                                      m_window->get_aspect_ratio(), 0.5f, 500.0f);
}

void App::run() {
  while (m_run) {
    auto now = std::chrono::high_resolution_clock::now();
    float dt = std::chrono::duration<float, std::chrono::seconds::period>(now - m_reference_time).count();
    m_reference_time = now;
    update(dt);
  }
}

void App::close_callback(GLFWwindow* window) {
  App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
  app->m_run = false;
}

void App::framebuffer_resize_callback(GLFWwindow* window, int width, int height) {
  App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
  app->m_window->set_size(width, height);
}

void App::mouse_position_callback(GLFWwindow* window, double xpos, double ypos) {
  App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
  static float prev_xpos = 0.0;
  static float prev_ypos = 0.0;
  float curr_xpos = -1.0 + xpos / static_cast<float>(app->m_window->get_width()) * 2.0;
  float curr_ypos = 1.0 - ypos / static_cast<float>(app->m_window->get_height()) * 2.0;

  if (glfwGetKey(app->m_window->get_instance(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS &&
      glfwGetMouseButton(app->m_window->get_instance(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
    app->m_camera->rotate(-glm::vec2(curr_xpos - prev_xpos, curr_ypos - prev_ypos), 0.01f);
  }

  prev_xpos = curr_xpos;
  prev_ypos = curr_ypos;
}

void App::scroll_callback(GLFWwindow* window, double, double yoffset) {
  App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
  if (glfwGetKey(app->m_window->get_instance(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
    app->m_camera->zoom(-yoffset, 0.01f);
  }
}