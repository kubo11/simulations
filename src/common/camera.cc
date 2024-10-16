#include "camera.hh"

Camera::Camera(glm::vec3 target_position, float pitch, float yaw, float dist, float min_dist, float max_dist, float fov,
               float aspect_ratio, float near_plane, float far_plane)
    : m_target_pos(target_position),
      m_pitch(pitch),
      m_yaw(yaw),
      m_min_dist(min_dist),
      m_max_dist(max_dist),
      m_dist(dist),
      m_world_up(glm::vec3(0.0f, 1.0f, 0.0f)),
      m_fov(fov),
      m_aspect_ratio(aspect_ratio),
      m_near_plane(near_plane),
      m_far_plane(far_plane),
      m_update_view(true),
      m_update_projection(true),
      m_update_camera_vectors(false),
      movement_sensitivity(100.0f),
      zoom_sensitivity(10.0f) {
  update_camera_vectors();
  get_view_matrix();
  get_projection_matrix();
}

void Camera::rotate(glm::vec2 offset, float dt) {
  if (glm::abs(offset.x) > glm::epsilon<float>()) m_yaw += offset.x * movement_sensitivity * dt;
  if (glm::abs(offset.y) > glm::epsilon<float>()) m_pitch += offset.y * movement_sensitivity * dt;
  clamp_angles();
  m_update_view = true;
  m_update_camera_vectors = true;
}

void Camera::zoom(float zoom, float dt) {
  m_dist += zoom * zoom_sensitivity * dt;
  clamp_distance();
  m_update_view = true;
  m_update_camera_vectors = true;
}

void Camera::update_camera_vectors() {
  m_pos = {m_target_pos.x + m_dist * cos(m_pitch) * sin(m_yaw), m_target_pos.y + m_dist * sin(m_pitch),
           m_target_pos.z + m_dist * cos(m_pitch) * cos(m_yaw)};
  m_front = glm::normalize(m_pos - m_target_pos);
  m_right = glm::normalize(glm::cross(m_front, m_world_up));
  m_up = glm::normalize(glm::cross(m_right, m_front));
}

glm::mat4 Camera::get_view_matrix() {
  if (m_update_camera_vectors) {
    m_update_camera_vectors = false;
    update_camera_vectors();
  }
  if (m_update_view) {
    m_update_view = false;
    m_view = glm::lookAt(m_pos, m_target_pos, m_up);
  }

  return m_view;
}

glm::mat4 Camera::get_projection_matrix() {
  if (m_update_projection) {
    m_update_projection = false;
    m_projection = glm::perspective(glm::radians(m_fov), m_aspect_ratio, m_near_plane, m_far_plane);
  }

  return m_projection;
}

void Camera::clamp_distance() { m_dist = std::clamp(m_dist, m_min_dist, m_max_dist); }

void Camera::clamp_angles() {
  m_pitch = std::clamp(m_pitch, glm::radians(-89.0f), glm::radians(89.0f));
  while (m_yaw > glm::pi<float>()) m_yaw -= 2.0f * glm::pi<float>();
  while (m_yaw < -glm::pi<float>()) m_yaw += 2.0f * glm::pi<float>();
}