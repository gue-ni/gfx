#pragma once

#include "transform.h"

namespace gfx
{
  class Camera : public Transform
  {
  protected:
    glm::mat4 m_projection;

  public:
    Camera() : Transform() {}
    glm::mat4 get_view_matrix() const;
    glm::mat4 get_projection_matrix() const;
    glm::mat4 set_projection_matrix(const glm::mat4& matrix);
  };
}
