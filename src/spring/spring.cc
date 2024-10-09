#include "spring.hh"

Spring::Spring(float spring_rest_length, float mass, float elasticity, float damping, float position, float velocity,
               std::unique_ptr<Function> anchor_position_function, std::unique_ptr<Function> field_force_function)
    : spring_rest_length(spring_rest_length),
      weight_mass(mass),
      elasticity_coef(elasticity),
      damping_coef(damping),
      anchor_position_function(std::move(anchor_position_function)),
      field_force_function(std::move(field_force_function)) {
  reset(position, velocity);
}

void Spring::update(float dt) {
  m_anchor_position = (*anchor_position_function)(m_t + dt);
  m_weight_position = m_weight_position + dt * m_weight_velocity;

  m_field_force = (*field_force_function)(m_t + dt);
  m_elasticity_force = elasticity_coef * (m_anchor_position - m_weight_position);
  m_damping_force = -damping_coef * m_weight_velocity;

  m_weight_velocity = m_weight_velocity + dt * (m_field_force + m_damping_force + m_elasticity_force) / weight_mass;
  m_weight_acceleration = (m_field_force + m_damping_force + m_elasticity_force) / weight_mass;

  m_t += dt;
}

void Spring::reset(float position, float velocity) {
  m_t = 0.0f;
  m_weight_position = position;
  m_anchor_position = m_weight_position + spring_rest_length;
  m_weight_velocity = velocity;
  m_weight_acceleration = 0.0f;
  m_field_force = 0.0f;
  m_elasticity_force = 0.0f;
  m_damping_force = 0.0f;
}

float Spring::get_t() const { return m_t; }

float Spring::get_anchor_position() const { return m_anchor_position; }

float Spring::get_weight_position() const { return m_weight_position; }

float Spring::get_weight_velocity() const { return m_weight_velocity; }

float Spring::get_weight_acceleration() const { return m_weight_acceleration; }

float Spring::get_field_force() const { return m_field_force; }

float Spring::get_elasticity_force() const { return m_elasticity_force; }

float Spring::get_damping_force() const { return m_damping_force; }