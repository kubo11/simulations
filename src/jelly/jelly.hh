#ifndef SIMULATIONS_JELLY
#define SIMULATIONS_JELLY

#include "pch.hh"

enum CollisionModel { FullVelocityDamping, VelocityComponentDamping };
struct Spring {
  glm::vec3& beg;
  glm::vec3& end;
  float& length;
  float& elasticity;

  glm::vec3 get_force() const;
};

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
  void set_collision_elasticity(float elasticity);
  void set_collision_model(CollisionModel model);

 private:
  float m_t = 0.0;
  float m_mass = 0.0;
  float m_damping_coef = 0.0;
  float m_inner_spring_elasticity_coef = 0.0;
  float m_frame_spring_elasticity_coef = 0.0;
  float m_size = 0.0;
  float m_distortion_amount = 0.0;
  bool m_compute_frame_springs = true;
  float m_control_frame_spring_base_length = 0.0f;
  float m_cardinal_inner_spring_base_length = 0.0f;
  float m_diagonal_inner_spring_base_legnth = 0.0f;

  float m_collision_elasticity_coef = 0.0f;
  CollisionModel m_collision_model = CollisionModel::FullVelocityDamping;
  std::array<float, 6> m_bounding_box_surfaces = {-3.0, 3.0, -3.0, 3.0, -3.0, 3.0};

  glm::vec3 m_frame_position = {0.0, 0.0, 0.0};
  glm::quat m_frame_orientation = {};
  glm::vec3 m_gravitational_acceleration = {0.0, 0.0, 0.0};
  std::array<glm::vec3, s_total_point_count> m_control_point_positions = {};
  std::array<glm::vec3, s_total_point_count> m_control_point_velocities = {};
  std::array<std::vector<Spring>, s_total_point_count> m_control_point_springs = {};
  std::array<glm::vec3, 8> m_frame_corner_points = {};
  std::unordered_map<int, Spring> m_control_frame_springs = {};

  std::pair<glm::vec3, glm::vec3> runge_kutta_for_control_point(unsigned int i, unsigned int j, unsigned int k, float dt) const;
  glm::vec3 get_force_for_control_point(unsigned int i, unsigned int j, unsigned int k, glm::vec3 pos_offset, glm::vec3 vel_offset) const;
  glm::vec3 get_force_along_control_frame_spring(unsigned int i, unsigned int j, unsigned int k) const;
  std::pair<glm::vec3, glm::vec3> check_and_apply_collisions(glm::vec3 p1, glm::vec3 p2, glm::vec3 v2);

  unsigned int get_control_point_idx(unsigned int i, unsigned int j, unsigned int k) const;

  void setup_springs();
  void update_frame_corner_points();
  void update_spring_lengths();
};

#endif  // SIMULATIONS_JELLY