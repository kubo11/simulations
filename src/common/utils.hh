#ifndef SIMULATIONS_COMMON_UTILS
#define SIMULATIONS_COMMON_UTILS

#include <glad/gl.h>
// #include <format>
#include <iostream>
#include <string>

#define PREVENT_COPY(class_name)          \
  class_name(const class_name&) = delete; \
  class_name& operator=(const class_name&) = delete;

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

inline void glCheckError_(const char* file, int line) {
  GLenum errorCode;
  while ((errorCode = glGetError()) != GL_NO_ERROR) {
    std::string error;
    switch (errorCode) {
      case GL_INVALID_ENUM:
        error = "INVALID_ENUM";
        break;
      case GL_INVALID_VALUE:
        error = "INVALID_VALUE";
        break;
      case GL_INVALID_OPERATION:
        error = "INVALID_OPERATION";
        break;
      case GL_STACK_OVERFLOW:
        error = "STACK_OVERFLOW";
        break;
      case GL_STACK_UNDERFLOW:
        error = "STACK_UNDERFLOW";
        break;
      case GL_OUT_OF_MEMORY:
        error = "OUT_OF_MEMORY";
        break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:
        error = "INVALID_FRAMEBUFFER_OPERATION";
        break;
    }
    std::cout << std::format("{} | {} ({})", error, file, line) << std::endl;
  }
  if (errorCode != GL_NO_ERROR) throw std::runtime_error("OpenGL error");
}

#define glCheckError() glCheckError_(__FILE__, __LINE__)

inline size_t glSizeofType(GLenum type) {
  switch (type) {
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
      return sizeof(GLbyte);
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
      return sizeof(GLshort);
    case GL_INT:
    case GL_UNSIGNED_INT:
      return sizeof(GLint);
    case GL_FLOAT:
      return sizeof(GLfloat);
    case GL_DOUBLE:
      return sizeof(GLdouble);
    case GL_HALF_FLOAT:
      return sizeof(GLhalf);
  }

  return 0;
}

inline void imgui_center_text(const std::string& text) {
  auto width = ImGui::GetContentRegionAvail().x;
  auto text_width = ImGui::CalcTextSize(text.c_str()).x;

  ImGui::SetCursorPosX((width - text_width) * 0.5f);
  ImGui::Text(text.c_str());
}

#endif  // SIMULATIONS_COMMON_UTILS