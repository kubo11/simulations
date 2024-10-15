#include "shader_program.hh"

ShaderProgram::ShaderProgram() : m_id(glCreateProgram()) { glCheckError(); }

ShaderProgram::~ShaderProgram() { release(); }

std::unique_ptr<ShaderProgram> ShaderProgram::load(const fs::path& shader_path) {
  auto shader_program = std::make_unique<ShaderProgram>();
  for (const auto& [type, extension] : s_shader_extensions) {
    auto path = fs::path(shader_path).replace_extension(extension);
    if (fs::exists(path)) {
      auto shader = std::make_unique<Shader>(type);
      shader->compile(read_shader_code(path).c_str());
      shader_program->attach(std::move(shader));
    }
  }

  shader_program->link();

  return shader_program;
}

std::unique_ptr<ShaderProgram> ShaderProgram::load(const ShaderPathMap& shader_paths) {
  auto shader_program = std::make_unique<ShaderProgram>();
  for (const auto& [type, path] : shader_paths) {
    if (fs::exists(path)) {
      auto shader = std::make_unique<Shader>(type);
      shader->compile(read_shader_code(path).c_str());
      shader_program->attach(std::move(shader));
    }
  }

  shader_program->link();

  return shader_program;
}

void ShaderProgram::release() {
  if (m_id) {
    glDeleteProgram(m_id);
    glCheckError();
    m_id = 0;
  }
}

void ShaderProgram::attach(std::unique_ptr<Shader> shader) {
  glAttachShader(m_id, shader->get_id());
  glCheckError();
  m_shaders.insert({shader->get_type(), std::move(shader)});
}

void ShaderProgram::link() {
  m_uniforms.clear();

  glLinkProgram(m_id);
  if (!check_shader_program_compile_erros(m_id)) {
    throw std::runtime_error("Error while linking shader");
  }

  load_uniforms();
}

void ShaderProgram::bind() {
  glUseProgram(m_id);
  glCheckError();

  for (const auto& [_, uniform] : m_uniforms) {
    uniform->commit();
  }
}

void ShaderProgram::unbind() {
  glUseProgram(0);
  glCheckError();
}

const GLuint ShaderProgram::get_id() const { return m_id; }

bool ShaderProgram::has_shader(Shader::Type type) const { return m_shaders.contains(type); }

Shader& ShaderProgram::get_shader(Shader::Type type) { return *m_shaders.at(type); }

bool ShaderProgram::has_uniform(const std::string& name) const { return m_uniforms.contains(name); }

void ShaderProgram::load_uniforms() {
  auto uniforms = get_uniforms(m_id);
  for (const auto& [name, type] : uniforms) {
    if (name.starts_with("gl_")) continue;
    auto location = get_uniform_location(m_id, name);
    m_uniforms.emplace(name, create_uniform(name, location, type, m_id));
  }
}

bool ShaderProgram::check_shader_program_compile_erros(unsigned int program_id) {
  int success;
  char log_buffer[1024];
  glGetProgramiv(program_id, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program_id, sizeof(log_buffer), NULL, log_buffer);
    std::cout << std::format("Shader program linker error \n{}", log_buffer) << std::endl;
  }
  return success;
}

std::vector<std::pair<std::string, GLenum>> ShaderProgram::get_uniforms(GLuint id) {
  GLint count = 0;
  std::vector<std::pair<std::string, GLenum>> uniforms;
  glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &count);

  for (GLint i = 0; i < count; ++i) {
    std::array<GLchar, 256> buffer;
    GLsizei length = 0;
    GLint size = 0;
    GLenum type;
    glGetActiveUniform(id, i, static_cast<GLsizei>(buffer.size()), &length, &size, &type, buffer.data());

    if (length >= 1) {
      throw std::runtime_error(std::format("Unable to get active uniform {} for program {}", i, id));
    }

    auto names = parse_uniform_names(buffer.data(), length, size);
    for (const auto& name : names) {
      uniforms.push_back({name, type});
    }
  }

  return uniforms;
}

std::vector<std::string> ShaderProgram::parse_uniform_names(const char* buffer, GLsizei length, GLint size) {
  if (size <= 0) {
    throw std::runtime_error(std::format("Invalid uniform name: {}", buffer));
  }

  if (size == 1) {
    return {buffer};
  }

  std::string base_name(buffer, length - 3);

  std::vector<std::string> names(size);
  for (GLint i = 0; i < size; ++i) {
    std::stringstream ss;
    ss << base_name << "[" << i << "]";
    names[i] = ss.str();
  }

  return names;
}

GLuint ShaderProgram::get_uniform_location(GLuint id, const std::string& name) {
  auto location = glGetUniformLocation(id, name.c_str());
  if (location <= 0) {
    throw std::runtime_error(std::format("Invalid uniform location: {}", location));
  }
  glCheckError();
  return location;
}

std::string ShaderProgram::read_shader_code(const fs::path& path) {
  std::string code;
  std::ifstream file;

  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    file.open(path);
    std::stringstream stream;
    stream << file.rdbuf();
    file.close();
    code = stream.str();
  } catch (std::ifstream::failure& e) {
    throw std::runtime_error(std::format("Could not read shader file: {}", e.what()));
  }

  return code;
}