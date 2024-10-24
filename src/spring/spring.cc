#include "spring.hh"

Spring::Spring(float mass, float elasticity, float damping, float position, float velocity,
               std::unique_ptr<Function> rest_position_function, std::unique_ptr<Function> field_force_function)
    : weight_mass(mass),
      elasticity_coef(elasticity),
      damping_coef(damping),
      rest_position_function(std::move(rest_position_function)),
      field_force_function(std::move(field_force_function)) {
  reset(position, velocity);
}

void Spring::update(float dt) {
  float k1x = m_weight_velocity;
  float k1v = (elasticity_coef * (*rest_position_function)(m_t) - elasticity_coef * m_weight_position - damping_coef * m_weight_velocity + (*field_force_function)(m_t)) / weight_mass;

  float k2x = m_weight_velocity + k1v * dt / 2.0;
  float k2v = (elasticity_coef * (*rest_position_function)(m_t + dt / 2.0) - elasticity_coef * (m_weight_position + k1x * dt / 2.0) - damping_coef * (m_weight_velocity + k1v * dt / 2.0) + (*field_force_function)(m_t + dt / 2.0)) / weight_mass;

  float k3x = m_weight_velocity + k2v * dt / 2.0;
  float k3v = (elasticity_coef * (*rest_position_function)(m_t + dt / 2.0) - elasticity_coef * (m_weight_position + k2x * dt / 2.0) - damping_coef * (m_weight_velocity + k2v * dt / 2.0) + (*field_force_function)(m_t + dt / 2.0)) / weight_mass;

  float k4x = m_weight_velocity + k3v * dt;
  float k4v = (elasticity_coef * (*rest_position_function)(m_t + dt) - elasticity_coef * (m_weight_position + k3x * dt) - damping_coef * (m_weight_velocity + k3v * dt) + (*field_force_function)(m_t + dt)) / weight_mass;

  m_weight_position += dt / 6.0 * (k1x + 2.0 * k2x + 2.0 * k3x + k4x);
  m_weight_velocity += dt / 6.0 * (k1v + 2.0 * k2v + 2.0 * k3v + k4v);

  m_t += dt;

  m_rest_position = (*rest_position_function)(m_t);
  m_field_force = (*field_force_function)(m_t);
  m_elasticity_force = elasticity_coef * (m_rest_position - m_weight_position);
  m_damping_force = -damping_coef * m_weight_velocity;

  m_weight_acceleration = (m_elasticity_force + m_damping_force + m_field_force) / weight_mass;
}

void Spring::reset(float position, float velocity) {
  m_t = 0.0f;
  m_weight_position = position;
  m_rest_position = (*rest_position_function)(m_t);
  m_weight_velocity = velocity;
  m_weight_acceleration = 0.0f;
  m_field_force = 0.0f;
  m_elasticity_force = 0.0f;
  m_damping_force = 0.0f;
}

float Spring::get_t() const { return m_t; }

float Spring::get_rest_position() const { return m_rest_position; }

float Spring::get_weight_position() const { return m_weight_position; }

float Spring::get_weight_velocity() const { return m_weight_velocity; }

float Spring::get_weight_acceleration() const { return m_weight_acceleration; }

float Spring::get_field_force() const { return m_field_force; }

float Spring::get_elasticity_force() const { return m_elasticity_force; }

float Spring::get_damping_force() const { return m_damping_force; }