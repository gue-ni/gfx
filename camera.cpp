#include "camera.h"

namespace gfx
{
glm::mat4 Camera::view_matrix() const { return glm::inverse(world_transform()); }

glm::mat4 Camera::projection_matrix() const { return m_projection; }

glm::mat4 Camera::view_projection_matrix() const { return projection_matrix() * view_matrix(); }

float Camera::near() const { return m_near; }

float Camera::far() const { return m_far; }

void Camera::set_near(float near)
{
  m_near = near;
  compute_projection_matrix();
}

void Camera::set_far(float far)
{
  m_far = far;
  compute_projection_matrix();
}

PerspectiveCamera::PerspectiveCamera() : PerspectiveCamera(glm::radians(45.0f), 16.0f / 9.0f, 1.0f, 1000.0f) {}

PerspectiveCamera::PerspectiveCamera(float fov, float aspect_ratio, float near, float far)
    : Camera(near, far), m_fov(fov), m_aspect_ratio(aspect_ratio)
{
  compute_projection_matrix();
}

float PerspectiveCamera::aspect_ratio() const { return m_aspect_ratio; }

float PerspectiveCamera::fov() const { return m_fov; }

void PerspectiveCamera::set_attributes(float fov, float aspect_ratio, float near, float far)
{
  m_fov = fov;
  m_aspect_ratio = aspect_ratio;
  m_near = near;
  m_far = far;
  compute_projection_matrix();
}

void PerspectiveCamera::set_aspect_ratio(float aspect_ratio)
{
  m_aspect_ratio = aspect_ratio;
  compute_projection_matrix();
}

void PerspectiveCamera::compute_projection_matrix()
{
  m_projection = glm::perspective(m_fov, m_aspect_ratio, m_near, m_far);
}

OrthoCamera::OrthoCamera() : OrthoCamera(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f) {}

OrthoCamera::OrthoCamera(float left, float right, float bottom, float top, float near, float far)
    : Camera(near, far), m_left(left), m_right(right), m_bottom(bottom), m_top(top)
{
}

void OrthoCamera::compute_projection_matrix()
{
  m_projection = glm::ortho(m_left, m_right, m_bottom, m_top, m_near, m_far);
}

}  // namespace gfx