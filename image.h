
#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <string>

namespace gfx
{
class Image
{
 public:
  enum Format : GLint { RED = GL_RED, RG = GL_RG, RGB = GL_RGB, RGBA = GL_RGBA };

  Image() : m_data(nullptr), m_width(0), m_height(0), m_channels(0) {}

  Image(unsigned char* data, int width, int height, int channels)
      : m_data(data), m_width(width), m_height(height), m_channels(channels)
  {
  }

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
  void read(const std::string& path, bool flip_vertically = false);
  void read_from_buffer(const unsigned char* buffer, int len);
  bool write(const std::string& path);
  bool loaded() const { return m_data != nullptr; }
  glm::u8vec4 pixel(int x, int y) const;
  glm::u8vec4 sample(const glm::vec2&) const;

 private:
  unsigned char* m_data = nullptr;
  int m_width, m_height, m_channels;
};
}  // namespace gfx