#pragma once

#include "transform.h"

namespace gfx
{
class Camera : public Transform
{
 protected:
  float m_near, m_far;
  glm::mat4 m_projection{};
  Camera() : m_near(0.0f), m_far(1000.0f) {}
  Camera(float near, float far) : m_near(near), m_far(far) {}
  virtual void compute_projection_matrix() = 0;

 public:
  virtual void set_aspect_ratio(float aspect_ratio) {}
  glm::mat4 view_matrix() const;
  glm::mat4 projection_matrix() const;
  glm::mat4 view_projection_matrix() const;
  float near() const;
  float far() const;
  void set_near(float near);
  void set_far(float far);
};

class PerspectiveCamera : public Camera
{
 public:
  PerspectiveCamera();
  PerspectiveCamera(float fov, float aspect_ratio, float near, float far);
  void set_aspect_ratio(float aspect_ratio) override;
  void set_attributes(float fov, float aspect_ratio, float near, float far);
  float aspect_ratio() const;
  float fov() const;

 private:
  float m_fov, m_aspect_ratio;
  void compute_projection_matrix() override;
};

class OrthoCamera : public Camera
{
 public:
  OrthoCamera();
  OrthoCamera(float left, float right, float bottom, float top, float near, float far);
  float left() const;
  float right() const;
  float top() const;
  float bottom() const;
  void set_left(float v);
  void set_right(float v);
  void set_top(float v);
  void set_bottom(float v);

 private:
  float m_left, m_right, m_bottom, m_top;

  void compute_projection_matrix() override;
};

}  // namespace gfx
