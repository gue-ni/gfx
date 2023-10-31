#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <vector>

namespace gfx {
class Transform {
protected:
  glm::vec3 m_local_position;
  glm::quat m_local_rotation;
  glm::vec3 m_local_scale;
  glm::mat4 m_local_transform, m_world_transform;

  Transform* m_parent = nullptr;
  std::vector<Transform*> m_children;

  glm::mat4 compute_local_transform() const {  
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), m_local_position);
    glm::mat4 rotation = glm::mat4(m_local_rotation);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_local_scale);
    return translate * rotation * scale;
  }

  void update_transform() {
    m_local_transform = compute_local_transform();
    m_world_transform = m_parent ? (m_parent->get_world_transform() * m_local_transform) : m_local_transform;
        
    for (Transform* child : m_children) {
      child->update_transform();
    }
  }

public:

  Transform()
    : Transform(glm::vec3(0.0f), glm::quat(glm::vec3(0.0f)), glm::vec3(1.0f))
  {}

  Transform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, Transform* parent = nullptr)
    : m_local_position(position), m_local_rotation(rotation), m_local_scale(scale), m_parent(parent)
  {
    update_transform();
  }  

  Transform* get_parent() const { return m_parent; }

  glm::vec3 get_local_position() const { return m_local_position; }
  glm::vec3 get_local_scale() const { return m_local_scale; }
  glm::quat get_local_rotation() const { return m_local_rotation; }

  void set_local_position(const glm::vec3& position) {
    m_local_position = position;
    update_transform();
  }

  void set_local_scale(const glm::vec3& scale) {
    m_local_scale = scale;
    update_transform();
  }

  void set_local_rotation(const glm::quat& rotation) {
    m_local_rotation = rotation;
    update_transform();
  }

 void set_local_transform(const glm::mat4& matrix) {
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

  glm::vec3 get_world_position() const { return glm::vec3(m_world_transform[3]); }
  glm::mat4 get_local_transform() const { return m_local_transform; }
  glm::mat4 get_world_transform() const { return m_world_transform; }

  void add_child(Transform* child) { m_children.push_back(child); }
};
}
