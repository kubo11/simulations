#ifndef SIMULATIONS_COMMON_CAMERA
#define SIMULATIONS_COMMON_CAMERA

#include "pch.hh"

class Camera {
 public:
  float movement_sensitivity;
  float zoom_sensitivity;

  Camera(glm::vec3 target_position, float pitch, float yaw, float dist, float min_dist, float max_dist, float fov,
         float aspect_ratio, float near_plane, float far_plane);
  ~Camera() = default;

  PREVENT_COPY(Camera);

  glm::mat4 get_view_matrix();
  glm::mat4 get_projection_matrix();

  void rotate(glm::vec2 offset, float dt);
  void zoom(float zoom, float dt);

  void set_rotation(glm::vec2 rotation);
  void set_distance(float dist);

  void set_min_dist(float min_dist);
  void set_max_dist(float max_dist);

  const float get_aspect_ratio() const { return m_aspect_ratio; }
  const float get_near_plane() const { return m_near_plane; }
  const float get_far_plane() const { return m_far_plane; }
  const float get_fov() const { return m_fov; }
  const float get_pitch() const { return m_pitch; }
  const float get_yaw() const { return m_yaw; }
  const glm::vec3 get_position() const { return m_pos; }
  const glm::vec3 get_front() const { return m_front; }
  const glm::vec3 get_up() const { return m_up; }

  void set_aspect_ratio(float aspect_ratio) {
    m_update_projection = true;
    m_aspect_ratio = aspect_ratio;
  }
  void set_near_plane(float near_plane) {
    m_update_projection = true;
    m_near_plane = near_plane;
  }
  void set_far_plane(float far_plane) {
    m_update_projection = true;
    m_far_plane = far_plane;
  }
  void set_fov(float fov) {
    m_update_projection = true;
    m_fov = fov;
  }

 private:
  float m_aspect_ratio;
  float m_near_plane;
  float m_far_plane;
  float m_fov;

  float m_yaw;
  float m_pitch;
  float m_dist;
  float m_min_dist;
  float m_max_dist;

  glm::vec3 m_pos;
  glm::vec3 m_target_pos;
  glm::vec3 m_front;
  glm::vec3 m_up;
  glm::vec3 m_right;
  glm::vec3 m_world_up;

  glm::mat4 m_view;
  bool m_update_view;
  glm::mat4 m_projection;
  bool m_update_projection;
  bool m_update_camera_vectors;

  void update_camera_vectors();
  void clamp_distance();
  void clamp_angles();
};

#endif  // SIMULATIONS_COMMON_CAMERA