#pragma once

#include <GL/glew.h>
#include <string>
#include <glm/glm.hpp>

class Image
{
public:
  enum Format : GLint
  {
    RED = GL_RED,
    RG = GL_RG,
    RGB = GL_RGB,
    RGBA = GL_RGBA
  };

  Image(const std::string &path, bool flip_vertically = false);
  ~Image();

  Image(const Image &other) = delete;
  Image &operator=(const Image &other) = delete;
  Image(Image &&other) noexcept;
  Image &operator=(Image &&other) noexcept;

  unsigned char *data() const;
  int width() const;
  int height() const;
  int channels() const;
  Format format() const;

private:
  unsigned char *m_data = nullptr;
  int m_width, m_height, m_channels;
};