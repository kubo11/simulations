#include "jelly.hh"

Jelly::Jelly() {
  reset();
}

void Jelly::update(float dt) {
  m_t += dt;
}

void Jelly::reset() {
  m_t = 0.0f;
}

float Jelly::get_t() const {
  return m_t;
}