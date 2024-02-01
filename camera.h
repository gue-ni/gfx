#pragma once

#include "transform.h"

namespace gfx
{
class Camera : public Transform
{
 protected:
  glm::mat4 m_projection{};
  float m_near{}, m_far{};

 public:
  Camera() : Transform() {}
  glm::mat4 view_matrix() const;
  glm::mat4 projection_matrix() const;
  void set_projection_matrix(const glm::mat4& matrix);
  void set_projection_matrix(float fov, float aspect_ratio, float near, float far);
  float near() const;
  float far() const;
};
}  // namespace gfx
