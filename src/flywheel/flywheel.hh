#ifndef SIMULATIONS_FLYWHEEL
#define SIMULATIONS_FLYWHEEL

#include "pch.hh"

#include "function.hh"

class Flywheel {
 public:
  Flywheel();

  void update(float dt);
  void reset();

  float add_error(float length);

  float get_t() const;
  float get_piston_position() const;
  float get_piston_velocity() const;
  float get_piston_acceleration() const;
  float get_length() const;
  float get_current_length() const;
  float get_radius() const;
  float get_angle() const;

  void set_angular_velocity(float angular_velocity);
  void set_length(float length);
  void set_radius(float radius);
  void set_standard_deviation(float standard_deviation);

 private:
  float m_t;
  float m_piston_position;
  float m_piston_velocity;
  float m_piston_acceleration;
  float m_angle;
  float m_angular_velocity;
  float m_length;
  float m_current_length;
  float m_radius;
  float m_standard_deviation;

  std::random_device m_rd{};
  std::mt19937 m_gen{m_rd()};
  std::normal_distribution<float> m_d;
};

#endif  // SIMULATIONS_FLYWHEEL