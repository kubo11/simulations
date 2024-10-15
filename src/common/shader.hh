#ifndef SIMULATIONS_COMMON_SHADER
#define SIMULATIONS_COMMON_SHADER

#include "pch.hh"

class Shader {
 public:
  enum class Type { Vertex, TessellationControl, TessellationEvaluation, Geometry, Fragment, Compute };

  Shader(Type type);
  ~Shader();

  PREVENT_COPY(Shader);

  void compile(const char* source);

  const GLuint get_id() const { return m_id; }
  const Type get_type() const { return m_type; }

 private:
  GLuint m_id;
  Type m_type;

  void release();
  static GLenum to_gl_shader_type(Type type);
  static bool check_shader_compile_erros(unsigned int shader_id);
};

#endif  // SIMULATIONS_COMMON_SHADER