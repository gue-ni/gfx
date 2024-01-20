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

constexpr glm::vec3 rgb(uint32_t hex)
{
  assert(hex <= 0xffffffU);
  uint32_t r = (hex & 0xff0000U) >> 16;
  uint32_t g = (hex & 0x00ff00U) >> 8;
  uint32_t b = (hex & 0x0000ffU) >> 0;
  return rgb(r, g, b);
}
}  // namespace gfx