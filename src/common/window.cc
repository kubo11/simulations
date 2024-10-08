#include "window.hh"

Window::Window(const std::string& title, unsigned int width, unsigned int height, glm::vec3 clear_color)
    : m_title(title), m_width(width), m_height(height), m_clear_color(glm::vec4(clear_color, 1.0f)) {
  if (glfwInit() == GLFW_FALSE) {
    throw std::runtime_error("GLFW cannot be initialized");
  }
  glfwSetErrorCallback(Window::glfw_error_callback);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif  // __APPLE__

  m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
  if (m_window == NULL) {
    throw std::runtime_error("GLFW cannot create a window instance");
  }

  glfwMakeContextCurrent(m_window);
}

Window::~Window() {
  glfwDestroyWindow(m_window);
  glfwTerminate();
  m_window = nullptr;
}

void Window::update() {
  glfwSwapBuffers(m_window);
  glfwPollEvents();
}

void Window::clear() {
  glClearColor(m_clear_color.r, m_clear_color.g, m_clear_color.b, m_clear_color.a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

const std::string& Window::get_title() const { return m_title; }

unsigned int Window::get_width() const { return m_width; }

unsigned int Window::get_height() const { return m_height; }

float Window::get_aspect_ratio() const { return static_cast<float>(m_width) / static_cast<float>(m_height); }

GLFWwindow* Window::get_instance() { return m_window; }

bool Window::is_key_pressed(int key) { return glfwGetKey(m_window, key) == GLFW_PRESS; }

bool Window::is_button_pressed(int button) { return glfwGetMouseButton(m_window, button) == GLFW_PRESS; }

void Window::set_size(unsigned int width, unsigned int height) {
  glViewport(0, 0, width, height);
  m_width = width;
  m_height = height;
}

void Window::glfw_error_callback(int error, const char* description) {
  throw std::runtime_error("GLFW:" + std::to_string(error) + " " + std::string(description));
}