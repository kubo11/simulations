#ifndef SIMULATIONS_COMMON_SHADER_PROGRAM
#define SIMULATIONS_COMMON_SHADER_PROGRAM

#include "pch.hh"

#include "shader.hh"
#include "uniform.hh"
#include "uniform_types.hh"

using UniformMap = std::unordered_map<std::string, std::unique_ptr<Uniform>>;
using ShaderMap = std::unordered_map<Shader::Type, std::unique_ptr<Shader>>;
using ShaderPathMap = std::unordered_map<Shader::Type, fs::path>;

class ShaderProgram {
 public:
  ShaderProgram();
  ~ShaderProgram();

  PREVENT_COPY(ShaderProgram);

  static std::unique_ptr<ShaderProgram> load(const fs::path &shader_path);
  static std::unique_ptr<ShaderProgram> load(const ShaderPathMap &shader_paths);

  void attach(std::unique_ptr<Shader> shader);
  void link();
  void bind();
  void unbind();

  const GLuint get_id() const;
  bool has_shader(Shader::Type type) const;
  Shader &get_shader(Shader::Type type);
  bool has_uniform(const std::string &name) const;

  template <typename T>
  void set_uniform_value(const std::string &name, const T &value) {
    if (!m_uniforms.contains(name)) {
      // throw std::runtime_error(std::format("Uniform {} doesn't exist", name.c_str()));
      return;
    }

    m_uniforms.at(name)->set_value(value);
  }

  template <typename T>
  void set_and_commit_uniform_value(const std::string &name, const T &value) {
    if (!m_uniforms.contains(name)) {
      // throw std::runtime_error(std::format("Uniform {} doesn't exist", name.c_str()));
      return;
    }

    m_uniforms.at(name)->set_value(value);
    m_uniforms.at(name)->commit();
  }

 private:
  GLuint m_id;
  ShaderMap m_shaders;
  UniformMap m_uniforms;

  inline static const std::unordered_map<Shader::Type, std::string> s_shader_extensions = {
      {Shader::Type::Vertex, ".vert"},
      {Shader::Type::TessellationControl, ".tesc"},
      {Shader::Type::TessellationEvaluation, ".tese"},
      {Shader::Type::Geometry, ".geom"},
      {Shader::Type::Fragment, ".frag"},
      {Shader::Type::Compute, ".comp"},
  };

  void release();
  void load_uniforms();
  bool check_shader_program_compile_erros(unsigned int program_id);
  static std::vector<std::pair<std::string, GLenum>> get_uniforms(GLuint id);
  static std::vector<std::string> parse_uniform_names(const char *buffer, GLsizei length, GLint size);
  static GLuint get_uniform_location(GLuint id, const std::string &name);
  static std::string read_shader_code(const fs::path &path);
};

#endif  // MGE_RENDER_API_SHADER_PROGRAM