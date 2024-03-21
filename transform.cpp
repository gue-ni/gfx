#include "transform.h"

namespace gfx
{
void Transform::update_transform()
{
  m_local_transform = compute_local_transform();
  m_world_transform = m_parent ? (m_parent->world_transform() * m_local_transform) : m_local_transform;

  for (Transform* child : m_children) {
    child->update_transform();
  }
}

Transform::Transform() : Transform(glm::vec3(0.0f), glm::quat(glm::vec3(0.0f)), glm::vec3(1.0f)) { update_transform(); }

Transform::Transform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, Transform* parent)
    : m_local_position(position), m_local_rotation(rotation), m_local_scale(scale), m_parent(parent)
{
  update_transform();
}

Transform* Transform::parent() const { return m_parent; }

std::vector<Transform*> Transform::children() const { return m_children; }

glm::vec3 Transform::local_position() const { return m_local_position; }

glm::vec3 Transform::local_scale() const { return m_local_scale; }

glm::quat Transform::local_rotation() const { return m_local_rotation; }

glm::mat4 Transform::compute_local_transform() const
{
  glm::mat4 translate = glm::translate(glm::mat4(1.0f), m_local_position);
  glm::mat4 rotation = glm::mat4(m_local_rotation);
  glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_local_scale);
  return translate * rotation * scale;
}

void Transform::set_parent(Transform* parent)
{
  m_parent = parent;
  update_transform();
}

void Transform::add_child(Transform* child)
{
  assert(child->parent() == nullptr);
  child->set_parent(this);
  m_children.push_back(child);
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

void Transform::set_local_rotation(const glm::vec3& rotation)
{
  m_local_rotation = glm::quat(rotation);
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
  m_local_rotation = rotation;
  m_local_scale = scale;
  update_transform();
}

glm::vec3 Transform::world_position() const { return glm::vec3(m_world_transform[3]); }

glm::quat Transform::world_rotation() const
{
  return m_parent ? (m_parent->world_rotation() * m_local_rotation) : m_local_rotation;
}

glm::mat4 Transform::local_transform() const { return m_local_transform; }

glm::mat4 Transform::world_transform() const { return m_world_transform; }

glm::vec3 Transform::transform_direction(const glm::vec3& direction) const
{
  return transform(glm::mat4(world_rotation()), direction);
}

glm::vec3 Transform::inverse_transform_direction(const glm::vec3& direction) const
{
  return inverse_transform(glm::mat4(world_rotation()), direction);
}

glm::vec3 Transform::transform_point(const glm::vec3& point) const { return transform(world_transform(), point); }

glm::vec3 Transform::inverse_transform_point(const glm::vec3& point) const
{
  return inverse_transform(world_transform(), point);
}

glm::vec3 Transform::local_x_axis() const { return transform_direction(glm::vec3(1.0f, 0.0f, 0.0f)); }

glm::vec3 Transform::local_y_axis() const { return transform_direction(glm::vec3(0.0f, 1.0f, 0.0f)); }

glm::vec3 Transform::local_z_axis() const { return transform_direction(glm::vec3(0.0f, 0.0f, 1.0f)); }

void Transform::visit(std::function<void(Transform*)> visitor) 
{
  visitor(this);

  for (Transform* child : children()) {
    child->visit(visitor);
  }
}

}  // namespace gfx