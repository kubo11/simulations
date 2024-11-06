#ifndef SIMULATIONS_SPRING
#define SIMULATIONS_SPRING

#include "pch.hh"

#include "function.hh"

class Spring {
 public:
  Spring();
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

  void set_weight_mass(float mass);
  void set_elasticity_coef(float elasticity);
  void set_damping_coef(float damping);
  void set_rest_position_function(std::unique_ptr<Function> func);
  void set_field_force_function(std::unique_ptr<Function> func);

 private:
  float m_t;
  float m_rest_position;
  float m_weight_position;
  float m_weight_velocity;
  float m_weight_acceleration;
  float m_field_force;
  float m_elasticity_force;
  float m_damping_force;

  float m_weight_mass;
  float m_elasticity_coef;
  float m_damping_coef;
  std::unique_ptr<Function> m_rest_position_function;
  std::unique_ptr<Function> m_field_force_function;
};

#endif  // SIMULATIONS_SPRING