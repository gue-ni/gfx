#pragma once

#include "transform.h"

namespace gfx {

class Camera : public Transform {
protected:
  glm::mat4 m_projection;

public:
  Camera() : Transform() {}
  glm::mat4 get_view_matrix() const { return glm::inverse(get_world_transform()); }
  glm::mat4 get_projection_matrix() const { return m_projection; }
};

}
