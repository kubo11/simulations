#include "shader.hh"

Shader::Shader(Shader::Type type) : m_id(glCreateShader(to_gl_shader_type(type))), m_type(type) { glCheckError(); }

Shader::~Shader() { release(); }

void Shader::release() {
  if (m_id) {
    glDeleteShader(m_id);
    glCheckError();
    m_id = 0;
  }
}

void Shader::compile(const char *source) {
  glShaderSource(m_id, 1, &source, nullptr);
  glCheckError();
  glCompileShader(m_id);
  if (!check_shader_compile_erros(m_id)) {
    throw std::runtime_error("Error while compiling shader");
  }
}

GLenum Shader::to_gl_shader_type(Shader::Type type) {
  switch (type) {
    case Shader::Type::Vertex:
      return GL_VERTEX_SHADER;
    case Shader::Type::TessellationControl:
      return GL_TESS_CONTROL_SHADER;
    case Shader::Type::TessellationEvaluation:
      return GL_TESS_EVALUATION_SHADER;
    case Shader::Type::Geometry:
      return GL_GEOMETRY_SHADER;
    case Shader::Type::Fragment:
      return GL_FRAGMENT_SHADER;
    case Shader::Type::Compute:
      return GL_COMPUTE_SHADER;

    default:
      return GL_NONE;
  }
}

bool Shader::check_shader_compile_erros(unsigned int shader_id) {
  int success;
  char log_buffer[1024];
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader_id, sizeof(log_buffer), NULL, log_buffer);
    // std::cout << std::format("Shader compilation error \n{}", log_buffer) << std::endl;
  }
  return success;
}