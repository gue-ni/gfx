#include "transform.h"

namespace gfx
{
void Transform::update_transform()
{
  m_local_transform = compute_local_transform();
  m_world_transform = m_parent ? (m_parent->get_world_transform() * m_local_transform) : m_local_transform;

  for (Transform* child : m_children) {
    child->update_transform();
  }
}

Transform::Transform() : Transform(glm::vec3(0.0f), glm::quat(glm::vec3(0.0f)), glm::vec3(1.0f)) {}

Transform::Transform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, Transform* parent)
    : m_local_position(position), m_local_rotation(rotation), m_local_scale(scale), m_parent(parent)
{
  update_transform();
}

Transform* Transform::get_parent() const { return m_parent; }

glm::vec3 Transform::get_local_position() const { return m_local_position; }

glm::vec3 Transform::get_local_scale() const { return m_local_scale; }

glm::quat Transform::get_local_rotation() const { return m_local_rotation; }

glm::mat4 Transform::compute_local_transform() const
{
  glm::mat4 translate = glm::translate(glm::mat4(1.0f), m_local_position);
  glm::mat4 rotation = glm::mat4(m_local_rotation);
  glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_local_scale);
  return translate * rotation * scale;
}

void Transform::set_local_scale(const glm::vec3& scale)
{
  m_local_scale = scale;
  update_transform();
}

void Transform::set_local_rotation(const glm::quat& rotation)
{
  m_local_rotation = rotation;
  update_transform();
}

void Transform::set_local_position(const glm::vec3& position)
{
  m_local_position = position;
  update_transform();
}

void Transform::set_local_transform(const glm::mat4& matrix)
{
  glm::quat rotation;
  glm::vec3 translation, skew, scale;
  glm::vec4 perspective;

  glm::decompose(matrix, scale, rotation, translation, skew, perspective);

  m_local_position = translation;
  // https://stackoverflow.com/a/40024726/11009152
  m_local_rotation = glm::conjugate(rotation);
  m_local_scale = scale;
  update_transform();
}

glm::vec3 Transform::get_world_position() const { return glm::vec3(m_world_transform[3]); }

glm::quat Transform::get_world_rotation() const
{
  return m_parent ? (m_parent->get_world_rotation() * m_local_rotation) : m_local_rotation;
}

glm::mat4 Transform::get_local_transform() const { return m_local_transform; }

glm::mat4 Transform::get_world_transform() const { return m_world_transform; }

glm::vec3 Transform::transform_direction(const glm::vec3& direction) const
{
  return transform(glm::mat4(get_world_rotation()), direction);
}

glm::vec3 Transform::inverse_transform_direction(const glm::vec3& direction) const
{
  return inverse_transform(glm::mat4(get_world_rotation()), direction);
}

glm::vec3 Transform::transform_point(const glm::vec3& point) const { return transform(get_world_transform(), point); }

glm::vec3 Transform::inverse_transform_point(const glm::vec3& point) const
{
  return inverse_transform(get_world_transform(), point);
}

}  // namespace gfx