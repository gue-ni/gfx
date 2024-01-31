#include "camera.h"

namespace gfx
{
glm::mat4 Camera::view_matrix() const { return glm::inverse(world_transform()); }

glm::mat4 Camera::projection_matrix() const { return m_projection; }

void Camera::set_projection_matrix(const glm::mat4 &matrix) { m_projection = matrix; }

}  // namespace gfx