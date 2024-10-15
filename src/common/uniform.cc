#include "uniform.hh"

Uniform::Uniform(const std::string& name, const GLint location, const GLenum type)
    : m_name(name), m_location(location), m_type(type), m_committed(true) {}

const std::string& Uniform::get_name() const { return m_name; }

const GLint Uniform::get_location() const { return m_location; }

const GLenum Uniform::get_type() const { return m_type; }

void Uniform::commit() {
  if (!m_committed) {
    commit_data();
    m_committed = true;
  }
}

bool Uniform::type_error(const char* name) {
  std::cout << std::format("Attempted to set shader uniform with wrong type ({}, instead of {}): {}", name,
                           get_type_name(), get_name())
            << std::endl;
  return false;
}