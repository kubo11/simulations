#include "jelly.hh"

Jelly::Jelly() {
  reset();
  setup_springs();
  update_frame_corner_points();
}

void Jelly::update(float dt) {
  std::array<glm::vec3, s_total_point_count> mass_positions = {};
  std::array<glm::vec3, s_total_point_count> mass_velocities = {};

  for (unsigned int i = 0; i < s_linear_point_count; ++i) {
    for (unsigned int j = 0; j < s_linear_point_count; ++j) {
      for (unsigned int k = 0; k < s_linear_point_count; ++k) {
        auto [position, velocity] = runge_kutta_for_control_point(i, j, k, dt);
        auto previous_position = get_control_point_position(i, j, k);
        auto [reflected_position, reflected_velocity] = check_and_apply_collisions(previous_position, position, velocity);
        mass_positions.at(get_control_point_idx(i, j, k)) = reflected_position;
        mass_velocities.at(get_control_point_idx(i, j, k)) = reflected_velocity;
      }
    }
  }

  m_control_point_positions = mass_positions;
  m_control_point_velocities = mass_velocities;
  m_t += dt;
}

void Jelly::reset() {
  m_t = 0.0;
  m_control_point_velocities.fill(glm::vec3{0.0, 0.0, 0.0});
  auto calculate_position = [size = m_size](unsigned int i) {
    return static_cast<float>(i) * size / 3.0 - size / 2.0;
  };
  for (unsigned int i = 0; i < s_linear_point_count; ++i) {
    for (unsigned int j = 0; j < s_linear_point_count; ++j) {
      for (unsigned int k = 0; k < s_linear_point_count; ++k) {
        unsigned int idx = get_control_point_idx(i, j, k);
        m_control_point_positions[idx] = glm::vec3{calculate_position(i), calculate_position(j), calculate_position(k)};
      }
    }
  }
}

void Jelly::distort() {
  std::random_device device;
  std::mt19937 rng(device());
  std::normal_distribution dist(0.0f, m_distortion_amount);

  for (int i = 0; i < s_total_point_count; ++i) {
    m_control_point_velocities[i] += glm::vec3{dist(rng), dist(rng), dist(rng)};
  }
}

float Jelly::get_t() const {
  return m_t;
}

float Jelly::get_size() const {
  return m_size;
}

const glm::vec3& Jelly::get_frame_position() const {
  return m_frame_position;
}

const std::array<glm::vec3, Jelly::s_total_point_count>& Jelly::get_control_point_positions() const {
  return m_control_point_positions;
}

const glm::vec3& Jelly::get_control_point_position(unsigned int i, unsigned int j, unsigned int k) const {
  return m_control_point_positions.at(get_control_point_idx(i, j, k));
}

const glm::vec3& Jelly::get_control_point_velocity(unsigned int i, unsigned int j, unsigned int k) const {
  return m_control_point_velocities.at(get_control_point_idx(i, j, k));
}

std::pair<glm::vec3, glm::vec3> Jelly::runge_kutta_for_control_point(unsigned int i, unsigned int j, unsigned int k, float dt) const {
  auto position = get_control_point_position(i, j, k);
  auto velocity = get_control_point_velocity(i, j, k);
  
  auto k1x = velocity;
  auto k1v = get_force_for_control_point(i, j, k, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}) / m_mass;

  auto k2x = velocity + k1v * dt / 2.0f;
  auto k2v = get_force_for_control_point(i, j, k, k1x * dt / 2.0f, k1v * dt / 2.0f) / m_mass;

  auto k3x = velocity + k2v * dt / 2.0f;
  auto k3v = get_force_for_control_point(i, j, k, k2x * dt / 2.0f, k2v * dt / 2.0f) / m_mass;

  auto k4x = velocity + k3v * dt;
  auto k4v = get_force_for_control_point(i, j, k, k3x * dt, k3v * dt) / m_mass;

  position += dt / 6.0f * (k1x + 2.0f * k2x + 2.0f * k3x + k4x);
  velocity += dt / 6.0f * (k1v + 2.0f * k2v + 2.0f * k3v + k4v);

  return {position, velocity};
}

glm::vec3 Jelly::get_force_for_control_point(unsigned int i, unsigned j, unsigned k, glm::vec3 pos_offset, glm::vec3 vel_offset) const {
  glm::vec3 position = get_control_point_position(i, j, k) + pos_offset;
  glm::vec3 velocity = m_control_point_velocities.at(get_control_point_idx(i, j, k)) + vel_offset;
  glm::vec3 force = m_gravitational_acceleration * m_mass - m_damping_coef * velocity;

  for (const auto& spring : m_control_point_springs[get_control_point_idx(i, j, k)]) {
    force += spring.get_force();
  }

  if (m_compute_frame_springs) {
    force += get_force_along_control_frame_spring(i, j, k);
  }

  return force;
}

std::pair<glm::vec3, glm::vec3> Jelly::check_and_apply_collisions(glm::vec3 p1, glm::vec3 p2, glm::vec3 v2) {
  if (p2.x < m_bounding_box_surfaces[0]) {
    auto v = p2 - p1;
    auto t = (m_bounding_box_surfaces[0] - p1.x) / v.x;
    auto p = p1 + v * t;
    auto pr = p + glm::reflect(p2-p, glm::vec3(1.0f, 0.0f, 0.0f));
    auto vr = glm::vec3{-m_collision_elasticity_coef*v2.x, v2.y, v2.z};
    if (m_collision_model == CollisionModel::FullVelocityDamping) {
      vr.y *= m_collision_elasticity_coef;
      vr.z *= m_collision_elasticity_coef;
    }
    return check_and_apply_collisions(p, pr, vr);
  }
  if (p2.x > m_bounding_box_surfaces[1]) {
    auto v = p2 - p1;
    auto t = (m_bounding_box_surfaces[1] - p1.x) / v.x;
    auto p = p1 + v * t;
    auto pr = p + glm::reflect(p2-p, glm::vec3(-1.0f, 0.0f, 0.0f));
    auto vr = glm::vec3{-m_collision_elasticity_coef*v2.x, v2.y, v2.z};
    if (m_collision_model == CollisionModel::FullVelocityDamping) {
      vr.y *= m_collision_elasticity_coef;
      vr.z *= m_collision_elasticity_coef;
    }
    return check_and_apply_collisions(p, pr, vr);
  }
  if (p2.y < m_bounding_box_surfaces[2]) {
    auto v = p2 - p1;
    auto t = (m_bounding_box_surfaces[2] - p1.y) / v.y;
    auto p = p1 + v * t;
    auto pr = p + glm::reflect(p2-p, glm::vec3(0.0f, 1.0f, 0.0f));
    auto vr = glm::vec3{v2.x, -m_collision_elasticity_coef*v2.y, v2.z};
    if (m_collision_model == CollisionModel::FullVelocityDamping) {
      vr.x *= m_collision_elasticity_coef;
      vr.z *= m_collision_elasticity_coef;
    }
    return check_and_apply_collisions(p, pr, vr);
  }
  if (p2.y > m_bounding_box_surfaces[3]) {
    auto v = p2 - p1;
    auto t = (m_bounding_box_surfaces[3] - p1.y) / v.y;
    auto p = p1 + v * t;
    auto pr = p + glm::reflect(p2-p, glm::vec3(0.0f, -1.0f, 0.0f));
    auto vr = glm::vec3{v2.x, -m_collision_elasticity_coef*v2.y, v2.z};
    if (m_collision_model == CollisionModel::FullVelocityDamping) {
      vr.x *= m_collision_elasticity_coef;
      vr.z *= m_collision_elasticity_coef;
    }
    return check_and_apply_collisions(p, pr, vr);
  }
  if (p2.z < m_bounding_box_surfaces[4]) {
    auto v = p2 - p1;
    auto t = (m_bounding_box_surfaces[4] - p1.z) / v.z;
    auto p = p1 + v * t;
    auto pr = p + glm::reflect(p2-p, glm::vec3(0.0f, 0.0f, 1.0f));
    auto vr = glm::vec3{v2.x, v2.y, -m_collision_elasticity_coef*v2.z};
    if (m_collision_model == CollisionModel::FullVelocityDamping) {
      vr.x *= m_collision_elasticity_coef;
      vr.y *= m_collision_elasticity_coef;
    }
    return check_and_apply_collisions(p, pr, vr);
  }
  if (p2.z > m_bounding_box_surfaces[5]) {
    auto v = p2 - p1;
    auto t = (m_bounding_box_surfaces[5] - p1.z) / v.z;
    auto p = p1 + v * t;
    auto pr = p + glm::reflect(p2-p, glm::vec3(0.0f, 0.0f, -1.0f));
    auto vr = glm::vec3{v2.x, v2.y, -m_collision_elasticity_coef*v2.z};
    if (m_collision_model == CollisionModel::FullVelocityDamping) {
      vr.x *= m_collision_elasticity_coef;
      vr.y *= m_collision_elasticity_coef;
    }
    return check_and_apply_collisions(p, pr, vr);
  }

  return {p2, v2};
}

unsigned int Jelly::get_control_point_idx(unsigned int i, unsigned int j, unsigned int k) const {
  return s_linear_point_count * s_linear_point_count * i + s_linear_point_count * j + k;
}

glm::vec3 Jelly::get_force_along_control_frame_spring(unsigned int i, unsigned int j, unsigned int k) const {
  if (!m_control_frame_springs.contains(get_control_point_idx(i, j, k))) return glm::vec3(0.0f);
  return m_control_frame_springs.at(get_control_point_idx(i, j, k)).get_force();
}

void Jelly::set_mass(float mass) {
  m_mass = mass;
}

void Jelly::set_damping(float damping) {
  m_damping_coef = damping;
}

void Jelly::set_inner_spring_elasticity(float elasticity) {
  m_inner_spring_elasticity_coef = elasticity;
}

void Jelly::set_frame_spring_elasticity(float elasticity) {
  m_frame_spring_elasticity_coef = elasticity;
}

void Jelly::set_size(float size) {
  m_size = size;
  update_frame_corner_points();
  update_spring_lengths();
}

void Jelly::toggle_frame_springs(bool toggle) {
  m_compute_frame_springs = toggle;
}

void Jelly::set_frame_position(const glm::vec3& position) {
  m_frame_position = position;
  update_frame_corner_points();
}

void Jelly::set_frame_orientation(const glm::quat& orientation) {
  m_frame_orientation = orientation;
  update_frame_corner_points();
}

void Jelly::set_gravitational_acceleration(const glm::vec3 acceleration) {
  m_gravitational_acceleration = acceleration;
}

void Jelly::set_distortion_amount(float distortion_amount) {
  m_distortion_amount = distortion_amount;
}

void Jelly::set_collision_elasticity(float elasticity) {
  m_collision_elasticity_coef = elasticity;
}

void Jelly::set_collision_model(CollisionModel model) {
  m_collision_model = model;
}

void Jelly::setup_springs() {
  update_spring_lengths();

  for (int i = 0; i < s_total_point_count; ++i) {
    int x1 = i / 16;
    int y1 = (i / 4) % 4;
    int z1 = i % 4;
    for (int j = 0; j < s_total_point_count; ++j) {
      int x2 = j / 16;
      int y2 = (j / 4) % 4;
      int z2 = j % 4;
      int dx = glm::abs(x2-x1);
      int dy = glm::abs(y2-y1);
      int dz = glm::abs(z2-z1); 
      if (dx <= 1 && dy <= 1 && dz <= 1 && i != j) {
        if (dx + dy + dz == 1) {
          m_control_point_springs[i].push_back(Spring{m_control_point_positions[i], m_control_point_positions[j], m_cardinal_inner_spring_base_length, m_inner_spring_elasticity_coef});
        }
        else if (dx + dy + dz == 2) {
          m_control_point_springs[i].push_back(Spring{m_control_point_positions[i], m_control_point_positions[j], m_diagonal_inner_spring_base_legnth, m_inner_spring_elasticity_coef});
        }
      }
    }
  }

  m_control_frame_springs.emplace(0, Spring{m_control_point_positions[0], m_frame_corner_points[0], m_control_frame_spring_base_length, m_frame_spring_elasticity_coef});
  m_control_frame_springs.emplace(3, Spring{m_control_point_positions[3], m_frame_corner_points[1], m_control_frame_spring_base_length, m_frame_spring_elasticity_coef});
  m_control_frame_springs.emplace(12, Spring{m_control_point_positions[12], m_frame_corner_points[2], m_control_frame_spring_base_length, m_frame_spring_elasticity_coef});
  m_control_frame_springs.emplace(15, Spring{m_control_point_positions[15], m_frame_corner_points[3], m_control_frame_spring_base_length, m_frame_spring_elasticity_coef});
  m_control_frame_springs.emplace(48, Spring{m_control_point_positions[48], m_frame_corner_points[4], m_control_frame_spring_base_length, m_frame_spring_elasticity_coef});
  m_control_frame_springs.emplace(51, Spring{m_control_point_positions[51], m_frame_corner_points[5], m_control_frame_spring_base_length, m_frame_spring_elasticity_coef});
  m_control_frame_springs.emplace(60, Spring{m_control_point_positions[60], m_frame_corner_points[6], m_control_frame_spring_base_length, m_frame_spring_elasticity_coef});
  m_control_frame_springs.emplace(63, Spring{m_control_point_positions[63], m_frame_corner_points[7], m_control_frame_spring_base_length, m_frame_spring_elasticity_coef});
}

glm::vec3 Spring::get_force() const {
  float l = glm::distance(beg, end) - length;
  return glm::normalize(end - beg) * elasticity * l;
}

void Jelly::update_frame_corner_points() {
  auto size = glm::sqrt(3.0f) * m_size / 2.0f;
  m_frame_corner_points[0] = m_frame_position + m_frame_orientation * (size * glm::normalize(glm::vec3(-1.0, -1.0, -1.0)));
  m_frame_corner_points[1] = m_frame_position + m_frame_orientation * (size * glm::normalize(glm::vec3(-1.0, -1.0, +1.0)));
  m_frame_corner_points[2] = m_frame_position + m_frame_orientation * (size * glm::normalize(glm::vec3(-1.0, +1.0, -1.0)));
  m_frame_corner_points[3] = m_frame_position + m_frame_orientation * (size * glm::normalize(glm::vec3(-1.0, +1.0, +1.0)));
  m_frame_corner_points[4] = m_frame_position + m_frame_orientation * (size * glm::normalize(glm::vec3(+1.0, -1.0, -1.0)));
  m_frame_corner_points[5] = m_frame_position + m_frame_orientation * (size * glm::normalize(glm::vec3(+1.0, -1.0, +1.0)));
  m_frame_corner_points[6] = m_frame_position + m_frame_orientation * (size * glm::normalize(glm::vec3(+1.0, +1.0, -1.0)));
  m_frame_corner_points[7] = m_frame_position + m_frame_orientation * (size * glm::normalize(glm::vec3(+1.0, +1.0, +1.0)));
}

void Jelly::update_spring_lengths() {
  m_cardinal_inner_spring_base_length = m_size / 3.0;
  m_diagonal_inner_spring_base_legnth = glm::sqrt(2.0) * m_size / 3.0;
}