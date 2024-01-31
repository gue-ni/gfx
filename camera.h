#pragma once

#include "transform.h"

namespace gfx
{
class Camera : public Transform
{
 protected:
  glm::mat4 m_projection{};

 public:
  Camera() : Transform() {}
  glm::mat4 view_matrix() const;
  glm::mat4 projection_matrix() const;
  void set_projection_matrix(const glm::mat4& matrix);
};
}  // namespace gfx
