#include "whirligig.hh"

Whirligig::Whirligig(glm::quat starting_orientation, float starting_angular_velocity, float cube_size, float cube_density, std::unique_ptr<Function> gravity_function) : m_cube_size(cube_size), m_cube_density(cube_density), m_gravity_function(std::move(gravity_function)) {
  reset(starting_orientation, starting_angular_velocity);
}

void Whirligig::update(float dt) {
  m_t += dt;
  m_gravity = (*m_gravity_function)(m_t);

  // TODO
}

void Whirligig::reset(glm::quat starting_orientation, float starting_angular_velocity) {
  std::lock_guard<std::mutex> guard(m_whirligig_mtx);

  m_t = 0.0f;
  m_orientation = starting_orientation;
  m_angular_velocity = starting_angular_velocity;
  m_gravity = (*m_gravity_function)(m_t);
}

void Whirligig::reset(float starting_incline, float starting_angular_velocity) {
  auto angle_rad = glm::radians(90.0f - starting_incline);
  auto axis = glm::normalize(glm::vec3(std::cos(angle_rad), std::sin(angle_rad), 0.0f));
  auto qq = glm::angleAxis(0.0f, axis);
  m_starting_incline = starting_incline;
  reset(glm::angleAxis(1e-6f, axis), m_angular_velocity);
}

float Whirligig::get_t() {
  std::lock_guard<std::mutex> guard(m_whirligig_mtx);
  return m_t;
}

glm::vec3 Whirligig::get_diagonal() {
  std::lock_guard<std::mutex> guard(m_whirligig_mtx);
  return glm::normalize(glm::vec3(m_orientation.x, m_orientation.y, m_orientation.z)) * m_cube_size * std::sqrt(3.0f);
}

float Whirligig::get_angular_velocity() {
  std::lock_guard<std::mutex> guard(m_whirligig_mtx);
  return m_angular_velocity;
}

float Whirligig::get_gravity() {
  std::lock_guard<std::mutex> guard(m_whirligig_mtx);
  return m_gravity;
}

glm::quat Whirligig::get_orientation() {
  std::lock_guard<std::mutex> guard(m_whirligig_mtx);
  return m_orientation;
}

float Whirligig::get_starting_incline() {
  std::lock_guard<std::mutex> guard(m_whirligig_mtx);
  return m_starting_incline;
}

float Whirligig::get_cube_size() {
  std::lock_guard<std::mutex> guard(m_whirligig_mtx);
  return m_cube_size;
}

void Whirligig::set_cube_size(float size) {
  std::lock_guard<std::mutex> guard(m_whirligig_mtx);
  m_cube_size = size;
}

void Whirligig::set_cube_density(float density) {
  std::lock_guard<std::mutex> guard(m_whirligig_mtx);
  m_cube_density = density;
}

void Whirligig::set_gravity_function(std::unique_ptr<Function> func) {
  std::lock_guard<std::mutex> guard(m_whirligig_mtx);
  m_gravity_function = std::move(func);
}