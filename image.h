
#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <string>
#include <filesystem>

namespace gfx
{
class Image
{
 public:
  enum Format : GLint { RED = GL_RED, RG = GL_RG, RGB = GL_RGB, RGBA = GL_RGBA };

  enum Sampling { NEAREST, LINEAR };

  Image();
  Image(unsigned char* data, int width, int height, int channels);
  ~Image();
  Image(const Image& other) = delete;
  Image& operator=(const Image& other) = delete;
  Image(Image&& other) noexcept;
  Image& operator=(Image&& other) noexcept;

  unsigned char* data() const;
  int width() const;
  int height() const;
  int channels() const;
  Format format() const;
  void load(const std::filesystem::path& path, bool flip_vertically = false);
  void load_from_memory(const unsigned char* buffer, int len);
  void load_from_array(unsigned char* buffer, int width, int height, int channels);
  bool write_png(const std::filesystem::path& path) const;
  bool is_loaded() const { return m_data != nullptr; }
  glm::u8vec4 pixel(int x, int y) const;
  glm::u8vec4 sample(const glm::vec2&, Sampling algorithm = NEAREST) const;

 private:
  bool m_allocated;
  unsigned char* m_data = nullptr;
  int m_width, m_height, m_channels;
};
}  // namespace gfx