#include "flywheel.hh"

Flywheel::Flywheel()
    : m_angular_velocity(0.0f), m_length(0.0f), m_radius(0.0f), m_standard_deviation(0.0f) {
  std::random_device m_rd{};
  std::mt19937 m_gen{m_rd()};

  reset();
}

void Flywheel::update(float dt) {
  float oldX = m_piston_position;
  float oldXt = m_piston_velocity;

  m_angle += m_angular_velocity * dt;
  m_angle = std::fmod(m_angle, 2.0f*std::numbers::pi);

  m_current_length = add_error(m_length);

  float px = std::cos(m_angle) * m_radius;
  float py = std::sin(m_angle) * m_radius;
  float s =  py / m_current_length;
  m_piston_position = std::sqrt(1.0f - s*s) * m_current_length + px;

  m_piston_velocity = (m_piston_position - oldX) / (dt);
  m_piston_acceleration = (m_piston_velocity - oldXt) / (dt);

  m_t += dt;
}

void Flywheel::reset() {
  m_t = 0.0f;
  m_piston_position = m_length + m_radius;
  m_piston_velocity = 0.0f;
  m_piston_acceleration = 0.0f;
  m_angle = 0.0f;
  m_current_length = add_error(m_length);
}

float Flywheel::add_error(float length) {
  std::normal_distribution m_d{0.0f, m_standard_deviation};
  return m_d(m_gen) + length;
}

float Flywheel::get_t() const {
  return m_t;
}

float Flywheel::get_piston_position() const {
  return m_piston_position;
}

float Flywheel::get_piston_velocity() const {
  return m_piston_velocity;
}

float Flywheel::get_piston_acceleration() const {
  return m_piston_acceleration;
}

float Flywheel::get_length() const {
  return m_length;
}

float Flywheel::get_radius() const {
  return m_radius;
}

float Flywheel::get_angle() const {
  return m_angle;
}

void Flywheel::set_angular_velocity(float angular_velocity) {
  m_angular_velocity = angular_velocity;
}

void Flywheel::set_length(float length) {
  m_length = m_current_length = length;
}

float Flywheel::get_current_length() const {
  return m_current_length;
}

void Flywheel::set_radius(float radius) {
  m_radius = radius;
}

void Flywheel::set_standard_deviation(float standard_deviation) {
  m_standard_deviation = standard_deviation;
}
