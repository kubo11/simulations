#ifndef SIMULATIONS_JELLY
#define SIMULATIONS_JELLY

#include "pch.hh"

class Jelly {
  static constexpr unsigned int s_linear_point_count = 4u;
  static constexpr unsigned int s_total_point_count = s_linear_point_count * s_linear_point_count * s_linear_point_count;

 public:
  Jelly();

  void update(float dt);
  void reset();
  void distort();

  float get_t() const;
  float get_size() const;
  const glm::vec3& get_frame_position() const;
  const std::array<glm::vec3, s_total_point_count>& get_control_point_positions() const;
  const glm::vec3& get_control_point_position(unsigned int i, unsigned int j, unsigned int k) const;
  const glm::vec3& get_control_point_velocity(unsigned int i, unsigned int j, unsigned int k) const;

  void set_mass(float mass);
  void set_damping(float damping);
  void set_inner_spring_elasticity(float elasticity);
  void set_frame_spring_elasticity(float elasticity);
  void set_size(float size);
  void toggle_frame_springs(bool toggle);
  void set_frame_position(const glm::vec3& position);
  void set_frame_orientation(const glm::quat& orientation);
  void set_gravitational_acceleration(const glm::vec3 acceleration);
  void set_distortion_amount(float distortion_amount);

 private:
  float m_t = 0.0;
  float m_mass = 0.0;
  float m_damping_coef = 0.0;
  float m_inner_spring_elasticity_coef = 0.0;
  float m_frame_spring_elasticity_coef = 0.0;
  float m_size = 0.0;
  float m_distortion_amount = 0.0;
  bool m_compute_frame_springs = true;
  glm::vec3 m_frame_position = {0.0, 0.0, 0.0};
  glm::quat m_frame_orientation = {};
  glm::vec3 m_gravitational_acceleration = {0.0, 0.0, 0.0};
  std::array<glm::vec3, s_total_point_count> m_control_point_positions = {};
  std::array<glm::vec3, s_total_point_count> m_control_point_velocities = {};

  std::pair<glm::vec3, glm::vec3> runge_kutta_for_control_point(unsigned int i, unsigned int j, unsigned int k, float dt) const;
  glm::vec3 get_force_for_control_point(unsigned int i, unsigned int j, unsigned int k, glm::vec3 pos_offset, glm::vec3 vel_offset) const;
  glm::vec3 get_elasticity_force_along_spring(glm::vec3 p1, glm::vec3 p2, float l0, float c) const;

  float get_cardinal_inner_spring_base_length() const;
  float get_diagonal_inner_spring_base_length() const;
  constexpr float get_frame_spring_base_length() const;

  unsigned int get_control_point_idx(unsigned int i, unsigned int j, unsigned int k) const;
};

#endif  // SIMULATIONS_JELLY