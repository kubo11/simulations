#ifndef SIMULATIONS_COMMON_WINDOW
#define SIMULATIONS_COMMON_WINDOW

#include "pch.hh"

class Window {
 public:
  Window(const std::string& title, unsigned int width, unsigned int height, glm::vec3 clear_color);
  ~Window();

  PREVENT_COPY(Window);

  void update();
  void clear();

  const std::string& get_title() const;
  unsigned int get_width() const;
  unsigned int get_height() const;
  float get_aspect_ratio() const;
  GLFWwindow* get_instance();

  bool is_key_pressed(int key);
  bool is_button_pressed(int button);

  void set_size(unsigned int width, unsigned int height);
  void set_clear_color(const glm::vec4& color);

  static void glfw_error_callback(int error, const char* description);

 private:
  std::string m_title;
  unsigned int m_width;
  unsigned int m_height;
  GLFWwindow* m_window;
  glm::vec4 m_clear_color;
};

#endif  // SIMULATIONS_COMMON_WINDOW