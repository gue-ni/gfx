
#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <GL/glew.h>

#include <filesystem>
#include <glm/glm.hpp>
#include <string>

namespace gfx
{
class Image
{
 public:
  enum Format : GLint { RED = GL_RED, RG = GL_RG, RGB = GL_RGB, RGBA = GL_RGBA };

  enum Sampling { NEAREST, LINEAR };

  Image() noexcept;
  Image(const unsigned char* buffer, int len) noexcept;

  Image(const Image& other) = delete;
  Image& operator=(const Image& that) = delete;

  Image(Image&& other) noexcept;
  Image& operator=(Image&& other) noexcept;

  ~Image() noexcept;

  unsigned char* data() const;
  int width() const;
  int height() const;
  int channels() const;

  Format format() const;

  bool load(const std::filesystem::path& path, bool flip_vertically = false);
  bool write_png(const std::filesystem::path& path) const;

  glm::u8vec4 pixel(int x, int y) const;
  glm::u8vec4 sample(const glm::vec2&, Sampling algorithm = NEAREST) const;

  bool is_valid() const;

 private:
  unsigned char* m_data = nullptr;
  int m_width, m_height, m_channels;

  void cleanup();
  void reset();
};
}  // namespace gfx
