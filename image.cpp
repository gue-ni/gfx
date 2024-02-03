#include "image.h"

#include <iostream>
#include <utility>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace gfx
{

Image::~Image()
{
  if (m_data != nullptr) {
    stbi_image_free(m_data);
  }
}

Image::Image(Image&& other) noexcept
    : m_data(std::exchange(other.m_data, nullptr)),
      m_width(other.width()),
      m_height(other.height()),
      m_channels(other.channels())
{
}

Image& Image::operator=(Image&& other) noexcept
{
  std::swap(m_data, other.m_data);
  m_width = other.width();
  m_height = other.height();
  m_channels = other.channels();
  return *this;
}

unsigned char* Image::data() const { return m_data; }

int Image::width() const { return m_width; }

int Image::height() const { return m_height; }

int Image::channels() const { return m_channels; }

Image::Format Image::format() const
{
  assert(1 <= m_channels && m_channels <= 4);
  static Format formats[] = {RED, RG, RGB, RGBA};
  return formats[m_channels - 1];
}

void Image::read(const std::string& path, bool flip_vertically)
{
  stbi_set_flip_vertically_on_load(flip_vertically);
  m_data = stbi_load(path.c_str(), &m_width, &m_height, &m_channels, 0);
}

void Image::read_from_buffer(const unsigned char* buffer, int len)
{
  m_data = stbi_load_from_memory(buffer, len, &m_width, &m_height, &m_channels, 3);
  m_channels = 3;
}

bool Image::write(const std::string& path) const
{
  return stbi_write_png(path.c_str(), m_width, m_height, m_channels, m_data, m_width * m_channels) == 1;
}

glm::u8vec4 Image::pixel(int x, int y) const
{
  assert(loaded());
  assert(0 <= x && x <= m_width);
  assert(0 <= y && y <= m_height);

  int i = (y * m_width + x) * 3;
  return glm::u8vec4(m_data[i + 0], m_data[i + 1], m_data[i + 2], 255U);
}

glm::u8vec4 Image::sample(const glm::vec2& uv) const
{
#if 1
  // nearest
  int x = static_cast<int>(uv.x * m_width);
  int y = static_cast<int>(uv.y * m_height);
  return pixel(x, y);
#else
  // bilinear interpolation
  assert(false);
  return glm::u8vec4();
#endif
}

}  // namespace gfx