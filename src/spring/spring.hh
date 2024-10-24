#ifndef SIMULATIONS_SPRING
#define SIMULATIONS_SPRING

#include "pch.hh"

#include "function.hh"

class Spring {
 public:
  float weight_mass;
  float elasticity_coef;
  float damping_coef;
  std::unique_ptr<Function> rest_position_function;
  std::unique_ptr<Function> field_force_function;

  Spring(float mass, float elasticity, float damping, float position, float velocity,
         std::unique_ptr<Function> rest_position_function, std::unique_ptr<Function> field_force_function);

  void update(float dt);
  void reset(float position = 0.0f, float velocity = 0.0f);

  float get_t() const;
  float get_rest_position() const;
  float get_weight_position() const;
  float get_weight_velocity() const;
  float get_weight_acceleration() const;
  float get_field_force() const;
  float get_elasticity_force() const;
  float get_damping_force() const;

 private:
  float m_t;
  float m_rest_position;
  float m_weight_position;
  float m_weight_velocity;
  float m_weight_acceleration;
  float m_field_force;
  float m_elasticity_force;
  float m_damping_force;
};

#endif  // SIMULATIONS_SPRING