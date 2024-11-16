#include "whirligig.hh"

Whirligig::Whirligig()
    : m_cube_size(0.0f), m_cube_density(0.0f), m_gravity_function(std::move(std::make_unique<ConstFunction>(0))) {
  reset(0.0f, 0.0f);
}

Whirligig::Whirligig(float starting_tilt, float starting_angular_velocity, float cube_size,
                     float cube_density, std::unique_ptr<Function> gravity_function)
    : m_cube_size(cube_size), m_cube_density(cube_density), m_gravity_function(std::move(gravity_function)) {
  reset(starting_tilt, starting_angular_velocity);
}

void Whirligig::update(float dt) {
  auto h = dt;
  auto k1w = m_angular_velocity;
  auto k1v = m_inertia_tensor_inv * (get_torque() + glm::cross(m_inertia_tensor * k1w, k1w));
  auto k2w = m_angular_velocity + k1v * h / 2.0f;
  auto k2v = m_inertia_tensor_inv * (get_torque() + glm::cross(m_inertia_tensor * k2w, k2w));
  auto k3w = m_angular_velocity + k2v * h / 2.0f;
  auto k3v = m_inertia_tensor_inv * (get_torque() + glm::cross(m_inertia_tensor * k3w, k3w));
  auto k4w = m_angular_velocity + k3v * h;
  auto k4v = m_inertia_tensor_inv * (get_torque() + glm::cross(m_inertia_tensor * k4w, k4w));

  m_angular_velocity += h / 6.0f * (k1v + 2.0f * k2v + 2.0f * k3v + k4v);

  auto k1q = m_orientation * glm::quat(0.0f, m_angular_velocity / 2.0f);
  auto k2q = (m_orientation + k1q * h / 2.0f) * glm::quat(0.0f, m_angular_velocity / 2.0f);
  auto k3q = (m_orientation + k2q * h / 2.0f) * glm::quat(0.0f, m_angular_velocity / 2.0f);
  auto k4q = (m_orientation + k3q * h) * glm::quat(0.0f, m_angular_velocity / 2.0f);

  m_orientation += h / 6.0f * (k1q + 2.0f * k2q + 2.0f * k3q + k4q);
  m_orientation = glm::normalize(m_orientation);

  m_t += dt;
  m_gravity = (*m_gravity_function)(m_t);
}

void Whirligig::reset(float starting_cube_tilt, float starting_angular_velocity) {
  m_t = 0.0f;
  m_tilt_quat = glm::angleAxis(glm::radians(starting_cube_tilt), glm::vec3(0.0f, 0.0f, 1.0f));
  m_orientation = m_tilt_quat * glm::normalize(glm::quat(1.0f, 1.0f, 1.0f, 1.0f));
  m_angular_velocity = starting_angular_velocity * glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f));
  m_gravity = (*m_gravity_function)(m_t);
  update_intertia_tensor(m_tilt_quat);
}

float Whirligig::get_t() const {
  return m_t;
}

glm::vec3 Whirligig::get_diagonal() const {
  return glm::normalize(glm::vec3(m_orientation.x, m_orientation.y, m_orientation.z)) * m_cube_size * std::sqrt(3.0f);
}

glm::vec3 Whirligig::get_angular_velocity() const {
  return m_angular_velocity;
}

float Whirligig::get_gravity() const {
  return m_gravity;
}

glm::quat Whirligig::get_orientation() const {
  return m_orientation;
}

float Whirligig::get_cube_size() const {
  return m_cube_size;
}

void Whirligig::set_cube_size(float size) {
  m_cube_size = size;
  update_intertia_tensor(m_tilt_quat);
}

void Whirligig::set_cube_density(float density) {
  m_cube_density = density;
  update_intertia_tensor(m_tilt_quat);
}

void Whirligig::set_gravity_function(std::unique_ptr<Function> func) {
  m_gravity_function = std::move(func);
  update_intertia_tensor(m_tilt_quat);
}

void Whirligig::set_gravity(bool gravity) {
  m_enable_gravity = gravity;
}

void Whirligig::update_intertia_tensor(glm::quat tilt_quat) {
  auto base_tensor = glm::mat3{+2.0f / 3.0f, -1.0f / 4.0f, -1.0f / 4.0f,
                               -1.0f / 4.0f, +2.0f / 3.0f, -1.0f / 4.0f,
                               -1.0f / 4.0f, -1.0f / 4.0f, +2.0f / 3.0f};
  m_inertia_tensor = std::pow(m_cube_size, 5.0f) * m_cube_density * base_tensor;
  m_inertia_tensor_inv = glm::inverse(m_inertia_tensor);
}

glm::vec3 Whirligig::get_torque() const {
  if (!m_enable_gravity) return glm::vec3(0.0f);
  auto q = m_orientation * glm::vec3(m_cube_size) / 2.0f;
  glm::vec3 g = m_gravity * glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
  g = glm::cross(q, g);
  glm::quat qN = glm::conjugate(m_orientation) * glm::quat(0, g) * m_orientation;
  return glm::vec3{qN.x, qN.y, qN.z};
}