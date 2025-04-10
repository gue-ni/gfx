#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <memory>
#include <vector>

// optional render information
class Drawable;

namespace gfx
{

inline glm::vec3 transform(const glm::mat4 &matrix, const glm::vec3 &vector)
{
  return glm::vec3(matrix * glm::vec4(vector, 1.0f));
}

inline glm::vec3 inverse_transform(const glm::mat4 &matrix, const glm::vec3 &vector)
{
  return glm::vec3(glm::inverse(matrix) * glm::vec4(vector, 1.0f));
}

class Transform
{
 protected:
  glm::vec3 m_local_position;
  glm::quat m_local_rotation;
  glm::vec3 m_local_scale;
  glm::mat4 m_local_transform, m_world_transform;

  Transform *m_parent = nullptr;
  std::vector<Transform *> m_children;

  glm::mat4 compute_local_transform() const;

  void update_transform();

 public:
  Transform();
  Transform(const glm::vec3 &position, const glm::quat &rotation, const glm::vec3 &scale = glm::vec3(1.0f));
  Transform(const glm::vec3 &position, const glm::quat &rotation, const glm::vec3 &scale, Transform *parent = nullptr);

  virtual ~Transform();

  std::shared_ptr<Drawable> drawable = nullptr;

  Transform *parent() const;
  std::vector<Transform *> children() const;

  glm::vec3 local_position() const;
  glm::vec3 local_scale() const;
  glm::quat local_rotation() const;

  void add_child(Transform *child);
  void set_parent(Transform *parent);
  void set_local_position(const glm::vec3 &position);
  void set_local_scale(const glm::vec3 &scale);
  void set_local_rotation(const glm::quat &rotation);
  void set_local_rotation(const glm::vec3 &rotation);
  void set_local_transform(const glm::mat4 &matrix);

  void look_at(const glm::vec3 &position, const glm::vec3 &target, const glm::vec3 &up);

  glm::vec3 world_position() const;
  glm::quat world_rotation() const;
  glm::mat4 local_transform() const;
  glm::mat4 world_transform() const;

  // rotation only
  glm::vec3 transform_direction(const glm::vec3 &direction) const;
  glm::vec3 inverse_transform_direction(const glm::vec3 &direction) const;

  // rotation and scale only
  // glm::vec3 transform_vector(const glm::vec& vector) const;
  // glm::vec3 inverse_transform_vector(const glm::vec& vector) const;

  // position, rotation and scale
  glm::vec3 transform_point(const glm::vec3 &point) const;
  glm::vec3 inverse_transform_point(const glm::vec3 &point) const;

  glm::vec3 local_x_axis() const;
  glm::vec3 local_y_axis() const;
  glm::vec3 local_z_axis() const;

  void visit(std::function<void(Transform *)> visitor);

  inline Transform *operator[](std::size_t index) { return m_children[index]; }
};

class Scene : public Transform
{
 public:
  void set_parent(Transform *parent) = delete;
};

}  // namespace gfx
