#pragma once

#include "transform.h"

namespace gfx
{
class Camera : public Transform
{
 protected:
  glm::mat4 m_projection{};

 public:
  virtual void set_aspect_ratio(float aspect_ratio) {}
  glm::mat4 view_matrix() const;
  glm::mat4 projection_matrix() const;
  glm::mat4 view_projection_matrix() const;
};

class PerspectiveCamera : public Camera
{
 public:
  PerspectiveCamera() : PerspectiveCamera(glm::radians(45.0f), 16.0f / 9.0f, 1.0f, 1000.0f) {}

  PerspectiveCamera(float fov, float aspect_ratio, float near, float far)
      : Camera(), m_fov(fov), m_aspect_ratio(aspect_ratio), m_near(near), m_far(far)
  {
    compute_projection_matrix();
  }

  void set_aspect_ratio(float aspect_ratio) override;
  void set_attributes(float fov, float aspect_ratio, float near, float far);
  float near() const;
  float far() const;
  float aspect_ratio() const;
  float fov() const;

 private:
  float m_near, m_far, m_fov, m_aspect_ratio;
  void compute_projection_matrix();
};

class OrthoCamera : public Camera
{
 public:
  OrthoCamera();

 private:
};

}  // namespace gfx
