#include "material.hh"

Material::Material(Color color, std::unique_ptr<Texture> normal) : m_color(color), m_normal(std::move(normal)) {}

Material::Material(std::unique_ptr<Texture> diffuse, std::unique_ptr<Texture> normal) : m_diffuse(std::move(diffuse)), m_normal(std::move(normal)) {}

void Material::bind(ShaderProgram& shader) {
  if (m_diffuse) {
    m_diffuse->bind(0u);
    shader.set_uniform_value("sampleDiffuse", true);
  }
  else {
    shader.set_uniform_value("color", m_color);
    shader.set_uniform_value("sampleDiffuse", false);
  }

  if (m_normal) {
    m_normal->bind(1u);
    shader.set_uniform_value("sampleNormal", true);
  }
  else {
    shader.set_uniform_value("sampleNormal", false);
  }
}

void Material::unbind() {
  if (m_diffuse) m_diffuse->unbind();
  if (m_normal) m_normal->unbind();
}