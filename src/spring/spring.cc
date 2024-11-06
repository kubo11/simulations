#include "spring.hh"

Spring::Spring()
    : m_weight_mass(0.0f),
      m_elasticity_coef(0.0f),
      m_damping_coef(0.0f),
      m_rest_position_function(std::move(std::make_unique<ConstFunction>(0))),
      m_field_force_function(std::move(std::make_unique<ConstFunction>(0))) {
  reset(0.0f, 0.0f);
}

Spring::Spring(float mass, float elasticity, float damping, float position, float velocity,
               std::unique_ptr<Function> rest_position_function, std::unique_ptr<Function> field_force_function)
    : m_weight_mass(mass),
      m_elasticity_coef(elasticity),
      m_damping_coef(damping),
      m_rest_position_function(std::move(rest_position_function)),
      m_field_force_function(std::move(field_force_function)) {
  reset(position, velocity);
}

void Spring::update(float dt) {
  float k1x = m_weight_velocity;
  float k1v = (m_elasticity_coef * (*m_rest_position_function)(m_t)-m_elasticity_coef * m_weight_position -
               m_damping_coef * m_weight_velocity + (*m_field_force_function)(m_t)) /
              m_weight_mass;

  float k2x = m_weight_velocity + k1v * dt / 2.0;
  float k2v = (m_elasticity_coef * (*m_rest_position_function)(m_t + dt / 2.0) -
               m_elasticity_coef * (m_weight_position + k1x * dt / 2.0) -
               m_damping_coef * (m_weight_velocity + k1v * dt / 2.0) + (*m_field_force_function)(m_t + dt / 2.0)) /
              m_weight_mass;

  float k3x = m_weight_velocity + k2v * dt / 2.0;
  float k3v = (m_elasticity_coef * (*m_rest_position_function)(m_t + dt / 2.0) -
               m_elasticity_coef * (m_weight_position + k2x * dt / 2.0) -
               m_damping_coef * (m_weight_velocity + k2v * dt / 2.0) + (*m_field_force_function)(m_t + dt / 2.0)) /
              m_weight_mass;

  float k4x = m_weight_velocity + k3v * dt;
  float k4v =
      (m_elasticity_coef * (*m_rest_position_function)(m_t + dt) - m_elasticity_coef * (m_weight_position + k3x * dt) -
       m_damping_coef * (m_weight_velocity + k3v * dt) + (*m_field_force_function)(m_t + dt)) /
      m_weight_mass;

  m_weight_position += dt / 6.0 * (k1x + 2.0 * k2x + 2.0 * k3x + k4x);
  m_weight_velocity += dt / 6.0 * (k1v + 2.0 * k2v + 2.0 * k3v + k4v);

  m_t += dt;

  m_rest_position = (*m_rest_position_function)(m_t);
  m_field_force = (*m_field_force_function)(m_t);
  m_elasticity_force = m_elasticity_coef * (m_rest_position - m_weight_position);
  m_damping_force = -m_damping_coef * m_weight_velocity;

  m_weight_acceleration = (m_elasticity_force + m_damping_force + m_field_force) / m_weight_mass;
}

void Spring::reset(float position, float velocity) {
  m_t = 0.0f;
  m_weight_position = position;
  m_rest_position = (*m_rest_position_function)(m_t);
  m_weight_velocity = velocity;
  m_weight_acceleration = 0.0f;
  m_field_force = 0.0f;
  m_elasticity_force = 0.0f;
  m_damping_force = 0.0f;
}

float Spring::get_t() const {
  return m_t;
}

float Spring::get_rest_position() const {
  return m_rest_position;
}

float Spring::get_weight_position() const {
  return m_weight_position;
}

float Spring::get_weight_velocity() const {
  return m_weight_velocity;
}

float Spring::get_weight_acceleration() const {
  return m_weight_acceleration;
}

float Spring::get_field_force() const {
  return m_field_force;
}

float Spring::get_elasticity_force() const {
  return m_elasticity_force;
}

float Spring::get_damping_force() const {
  return m_damping_force;
}

void Spring::set_weight_mass(float mass) {
  m_weight_mass = mass;
}

void Spring::set_elasticity_coef(float elasticity) {
  m_elasticity_coef = elasticity;
}

void Spring::set_damping_coef(float damping) {
  m_damping_coef = damping;
}

void Spring::set_rest_position_function(std::unique_ptr<Function> func) {
  m_rest_position_function = std::move(func);
}

void Spring::set_field_force_function(std::unique_ptr<Function> func) {
  m_field_force_function = std::move(func);
}
