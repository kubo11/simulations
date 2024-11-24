#include "jelly.hh"

Jelly::Jelly() {
  reset();
}

void Jelly::update(float dt) {
  std::array<glm::vec3, s_total_point_count> mass_positions = {};
  std::array<glm::vec3, s_total_point_count> mass_velocities = {};

  for (unsigned int i = 0; i < s_linear_point_count; ++i) {
    for (unsigned int j = 0; j < s_linear_point_count; ++j) {
      for (unsigned int k = 0; k < s_linear_point_count; ++k) {
        auto [position, velocity] = runge_kutta_for_control_point(i, j, k, dt);
        mass_positions.at(get_control_point_idx(i, j, k)) = position;
        mass_velocities.at(get_control_point_idx(i, j, k)) = velocity;
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

  if (i > 0) {
    force += get_elasticity_force_along_spring(position, get_control_point_position(i-1, j, k), get_cardinal_inner_spring_base_length(), m_inner_spring_elasticity_coef);
  }

  if (j > 0) {
    force += get_elasticity_force_along_spring(position, get_control_point_position(i, j-1, k), get_cardinal_inner_spring_base_length(), m_inner_spring_elasticity_coef);
  }

  if (k > 0) {
    force += get_elasticity_force_along_spring(position, get_control_point_position(i, j, k-1), get_cardinal_inner_spring_base_length(), m_inner_spring_elasticity_coef);
  }

  if (i < s_linear_point_count - 1) {
    force += get_elasticity_force_along_spring(position, get_control_point_position(i+1, j, k), get_cardinal_inner_spring_base_length(), m_inner_spring_elasticity_coef);
  }

  if (j < s_linear_point_count - 1) {
    force += get_elasticity_force_along_spring(position, get_control_point_position(i, j+1, k), get_cardinal_inner_spring_base_length(), m_inner_spring_elasticity_coef);
  }

  if (k < s_linear_point_count - 1) {
    force += get_elasticity_force_along_spring(position, get_control_point_position(i, j, k+1), get_cardinal_inner_spring_base_length(), m_inner_spring_elasticity_coef);
  }

  if (i > 0 && j > 0) {
    force += get_elasticity_force_along_spring(position, get_control_point_position(i-1, j-1, k), get_diagonal_inner_spring_base_length(), m_inner_spring_elasticity_coef);
  }

  if (j > 0 && k > 0) {
    force += get_elasticity_force_along_spring(position, get_control_point_position(i, j-1, k-1), get_diagonal_inner_spring_base_length(), m_inner_spring_elasticity_coef);
  }

  if (i > 0 && k > 0) {
    force += get_elasticity_force_along_spring(position, get_control_point_position(i-1, j, k-1), get_diagonal_inner_spring_base_length(), m_inner_spring_elasticity_coef);
  }

  if (i < s_linear_point_count - 1 && j < s_linear_point_count - 1) {
    force += get_elasticity_force_along_spring(position, get_control_point_position(i+1, j+1, k), get_diagonal_inner_spring_base_length(), m_inner_spring_elasticity_coef);
  }

  if (j < s_linear_point_count - 1 && k < s_linear_point_count - 1) {
    force += get_elasticity_force_along_spring(position, get_control_point_position(i, j+1, k+1), get_diagonal_inner_spring_base_length(), m_inner_spring_elasticity_coef);
  }

  if (i < s_linear_point_count - 1 && k < s_linear_point_count - 1) {
    force += get_elasticity_force_along_spring(position, get_control_point_position(i+1, j, k+1), get_diagonal_inner_spring_base_length(), m_inner_spring_elasticity_coef);
  }

  if (i > 0 && j < s_linear_point_count - 1) {
    force += get_elasticity_force_along_spring(position, get_control_point_position(i-1, j+1, k), get_diagonal_inner_spring_base_length(), m_inner_spring_elasticity_coef);
  }

  if (j > 0 && k < s_linear_point_count - 1) {
    force += get_elasticity_force_along_spring(position, get_control_point_position(i, j-1, k+1), get_diagonal_inner_spring_base_length(), m_inner_spring_elasticity_coef);
  }

  if (i > 0 && k < s_linear_point_count - 1) {
    force += get_elasticity_force_along_spring(position, get_control_point_position(i-1, j, k+1), get_diagonal_inner_spring_base_length(), m_inner_spring_elasticity_coef);
  }

  if (i < s_linear_point_count - 1 && j > 0) {
    force += get_elasticity_force_along_spring(position, get_control_point_position(i+1, j-1, k), get_diagonal_inner_spring_base_length(), m_inner_spring_elasticity_coef);
  }

  if (j < s_linear_point_count - 1 && k > 0) {
    force += get_elasticity_force_along_spring(position, get_control_point_position(i, j+1, k-1), get_diagonal_inner_spring_base_length(), m_inner_spring_elasticity_coef);
  }

  if (i < s_linear_point_count - 1 && k > 0) {
    force += get_elasticity_force_along_spring(position, get_control_point_position(i+1, j, k-1), get_diagonal_inner_spring_base_length(), m_inner_spring_elasticity_coef);
  }

  if (!m_compute_frame_springs) return force;

  auto size = glm::sqrt(3.0f) * m_size / 2.0f;
  if (i == 0 && j == 0 && k == 0) {
    auto frame_point = m_frame_position + m_frame_orientation * (size * glm::normalize(glm::vec3(-1.0, -1.0, -1.0)));
    force += get_elasticity_force_along_spring(position, frame_point, get_frame_spring_base_length(), m_frame_spring_elasticity_coef);
  }

  if (i == s_linear_point_count - 1 && j == 0 && k == 0) {
    auto frame_point = m_frame_position + m_frame_orientation * (size * glm::normalize(glm::vec3(1.0, -1.0, -1.0)));
    force += get_elasticity_force_along_spring(position, frame_point, get_frame_spring_base_length(), m_frame_spring_elasticity_coef);
  }

  if (i == 0 && j == s_linear_point_count - 1 && k == 0) {
    auto frame_point = m_frame_position + m_frame_orientation * (size * glm::normalize(glm::vec3(-1.0, 1.0, -1.0)));
    force += get_elasticity_force_along_spring(position, frame_point, get_frame_spring_base_length(), m_frame_spring_elasticity_coef);
  }

  if (i == 0 && j == 0 && k == s_linear_point_count - 1) {
    auto frame_point = m_frame_position + m_frame_orientation * (size * glm::normalize(glm::vec3(-1.0, -1.0, 1.0)));
    force += get_elasticity_force_along_spring(position, frame_point, get_frame_spring_base_length(), m_frame_spring_elasticity_coef);
  }

  if (i == s_linear_point_count - 1 && j == s_linear_point_count - 1 && k == 0) {
    auto frame_point = m_frame_position + m_frame_orientation * (size * glm::normalize(glm::vec3(1.0, 1.0, -1.0)));
    force += get_elasticity_force_along_spring(position, frame_point, get_frame_spring_base_length(), m_frame_spring_elasticity_coef);
  }

  if (i == 0 && j == s_linear_point_count - 1 && k == s_linear_point_count - 1) {
    auto frame_point = m_frame_position + m_frame_orientation * (size * glm::normalize(glm::vec3(-1.0, 1.0, 1.0)));
    force += get_elasticity_force_along_spring(position, frame_point, get_frame_spring_base_length(), m_frame_spring_elasticity_coef);
  }

  if (i == s_linear_point_count - 1 && j == 0 && k == s_linear_point_count - 1) {
    auto frame_point = m_frame_position + m_frame_orientation * (size * glm::normalize(glm::vec3(1.0, -1.0, 1.0)));
    force += get_elasticity_force_along_spring(position, frame_point, get_frame_spring_base_length(), m_frame_spring_elasticity_coef);
  }

  if (i == s_linear_point_count - 1 && j == s_linear_point_count - 1 && k == s_linear_point_count - 1) {
    auto frame_point = m_frame_position + m_frame_orientation * (size * glm::normalize(glm::vec3(1.0, 1.0, 1.0)));
    force += get_elasticity_force_along_spring(position, frame_point, get_frame_spring_base_length(), m_frame_spring_elasticity_coef);
  }

  return force;
}

float Jelly::get_cardinal_inner_spring_base_length() const {
  return m_size / 3.0;
}

float Jelly::get_diagonal_inner_spring_base_length() const {
  return glm::sqrt(2.0) * m_size / 3.0;
}

constexpr float Jelly::get_frame_spring_base_length() const {
  return 0.0;
}

unsigned int Jelly::get_control_point_idx(unsigned int i, unsigned int j, unsigned int k) const {
  return s_linear_point_count * s_linear_point_count * i + s_linear_point_count * j + k;
}

glm::vec3 Jelly::get_elasticity_force_along_spring(glm::vec3 p1, glm::vec3 p2, float l0, float c) const {
  float l = glm::distance(p1, p2) - l0;
  return glm::normalize(p2 - p1) * c * l;
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
}

void Jelly::toggle_frame_springs(bool toggle) {
  m_compute_frame_springs = toggle;
}

void Jelly::set_frame_position(const glm::vec3& position) {
  m_frame_position = position;
}

void Jelly::set_frame_orientation(const glm::quat& orientation) {
  m_frame_orientation = orientation;
}

void Jelly::set_gravitational_acceleration(const glm::vec3 acceleration) {
  m_gravitational_acceleration = acceleration;
}

void Jelly::set_distortion_amount(float distortion_amount) {
  m_distortion_amount = distortion_amount;
}