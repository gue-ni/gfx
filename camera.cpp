#include "camera.h"

namespace gfx
{
glm::mat4 Camera::view_matrix() const { return glm::inverse(world_transform()); }

glm::mat4 Camera::projection_matrix() const { return m_projection; }

glm::mat4 Camera::view_projection_matrix() const { return projection_matrix() * view_matrix(); }

void Camera::set_projection_matrix(const glm::mat4 &matrix) { m_projection = matrix; }

float Camera::far() const { return m_far; }

float Camera::near() const { return m_near; }

void Camera::set_projection_matrix(float fov, float aspect_ratio, float near, float far)
{
  m_near = near;
  m_far = far;
  m_projection = glm::perspective(fov, aspect_ratio, near, far);
}

}  // namespace gfx