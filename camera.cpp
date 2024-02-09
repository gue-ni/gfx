#include "camera.h"

namespace gfx
{
glm::mat4 Camera::view_matrix() const { return glm::inverse(world_transform()); }

glm::mat4 Camera::projection_matrix() const { return m_projection; }

glm::mat4 Camera::view_projection_matrix() const { return projection_matrix() * view_matrix(); }

float Camera::far() const { return m_far; }

float Camera::near() const { return m_near; }

float Camera::aspect_ratio() const { return m_aspect_ratio; }

float Camera::fov() const { return m_fov; }

void Camera::set_attributes(float fov, float aspect_ratio, float near, float far)
{
  m_fov = fov;
  m_aspect_ratio = aspect_ratio;
  m_near = near;
  m_far = far;
  compute_projection_matrix();
}

void Camera::set_aspect_ratio(float aspect_ratio)
{
  m_aspect_ratio = aspect_ratio;
  compute_projection_matrix();
}

void Camera::compute_projection_matrix() { m_projection = glm::perspective(m_fov, m_aspect_ratio, m_near, m_far); }

}  // namespace gfx