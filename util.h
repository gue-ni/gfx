#pragma once

#include <cassert>
#include <glm/glm.hpp>

namespace gfx
{

// [0, 255] -> [0, 1]
template <typename T>
constexpr glm::vec3 rgb(T r, T g, T b)
{
  return glm::vec3(static_cast<float>(r), static_cast<float>(g), static_cast<float>(b)) / 255.0f;
}

template <typename T>
constexpr glm::vec4 rgba(T r, T g, T b, T a)
{
  return glm::vec4(static_cast<float>(r), static_cast<float>(g), static_cast<float>(b), static_cast<float>(a)) / 255.0f;
}

constexpr glm::vec3 rgb(uint32_t hex)
{
  assert(hex <= 0xffffffU);
  uint32_t r = (hex & 0xff0000U) >> 16;
  uint32_t g = (hex & 0x00ff00U) >> 8;
  uint32_t b = (hex & 0x0000ffU) >> 0;
  return rgb(r, g, b);
}

constexpr glm::vec4 rgba(uint32_t hex)
{
  uint32_t r = (hex & 0xff000000U) >> 24;
  uint32_t g = (hex & 0x00ff0000U) >> 16;
  uint32_t b = (hex & 0x0000ff00U) >> 8;
  uint32_t a = (hex & 0x000000ffU) >> 0;
  return rgba(r, g, b, a);
}

// a selection of colors
namespace color
{

constexpr glm::vec3 MAGENTA = gfx::rgb(0xf700f7);

constexpr glm::vec3 LEARNOPENGL = gfx::rgb(0x334C4C);

}  // namespace color

}  // namespace gfx