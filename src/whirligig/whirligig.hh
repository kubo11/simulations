#ifndef SIMULATIONS_WHIRLIGIG
#define SIMULATIONS_WHIRLIGIG

#include "pch.hh"

#include "function.hh"

class Whirligig {
 public:
  Whirligig();

  Whirligig(float starting_tilt, float starting_angular_velocity, float cube_size, float cube_density,
            std::unique_ptr<Function> gravity_function);

  void update(float dt);
  void reset(float starting_cube_tilt, float starting_angular_velocity);

  float get_t() const;
  glm::vec3 get_diagonal() const;
  glm::vec3 get_angular_velocity() const;
  float get_gravity() const;
  glm::quat get_orientation() const;
  float get_cube_size() const;

  void set_cube_size(float size);
  void set_cube_density(float density);
  void set_gravity_function(std::unique_ptr<Function> func);
  void set_gravity(bool gravity);

 private:
  float m_t;
  float m_cube_size = 1.0f;
  float m_cube_density = 1.0f;
  bool m_enable_gravity = true;
  glm::quat m_orientation;
  glm::quat m_tilt_quat = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
  glm::vec3 m_angular_velocity = {0.0f, 0.0f, 0.0f};
  float m_gravity = 0.0f;
  glm::mat3 m_inertia_tensor = glm::mat3(0.0f);
  glm::mat3 m_inertia_tensor_inv = glm::mat3(0.0f);
  std::unique_ptr<Function> m_gravity_function;

  void update_intertia_tensor(glm::quat tilt_quat);
  glm::vec3 get_torque() const;
};

#endif  // SIMULATIONS_WHIRLIGIG