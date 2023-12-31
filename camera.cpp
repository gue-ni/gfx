#include "camera.h"

namespace gfx
{
  glm::mat4 Camera::get_view_matrix() const
  {
    return glm::inverse(get_world_transform());
  }

  glm::mat4 Camera::get_projection_matrix() const
  {
    return m_projection;
  }
  void Camera::set_projection_matrix(const glm::mat4 &matrix)
  {
    m_projection = matrix;
  }
}