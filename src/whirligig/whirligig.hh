#ifndef SIMULATIONS_WHIRLIGIG
#define SIMULATIONS_WHIRLIGIG

#include "pch.hh"

#include "function.hh"

class Whirligig {
 public:
  Whirligig(glm::quat starting_orientation, float starting_angular_velocity, float cube_size, float cube_density, std::unique_ptr<Function> gravity_function);

  void update(float dt);
  void reset(glm::quat starting_orientation, float starting_angular_velocity);
  void reset(float starting_incline, float starting_angular_velocity);

  float get_t();
  glm::vec3 get_diagonal();
  float get_angular_velocity();
  float get_gravity();
  glm::quat get_orientation();
  float get_starting_incline();
  float get_cube_size();

  void set_cube_size(float size);
  void set_cube_density(float density);
  void set_gravity_function(std::unique_ptr<Function> func);

 private:
  float m_t;
  float m_cube_size = 1.0f;
  float m_cube_density = 1.0f;
  glm::quat m_orientation;
  float m_angular_velocity = 1.0f;
  float m_gravity = 0.0f;
  float m_starting_incline = 0.0f;
  std::unique_ptr<Function> m_gravity_function;

  std::mutex m_whirligig_mtx;
};

#endif  // SIMULATIONS_WHIRLIGIG