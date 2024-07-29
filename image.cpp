#include "image.h"

#include <iostream>
#include <utility>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

namespace gfx
{

Image::Image() : m_data(nullptr), m_width(0), m_height(0), m_channels(0), m_allocated(false) {}

Image::Image(unsigned char* data, int width, int height, int channels)
    : m_data(data), m_width(width), m_height(height), m_channels(channels), m_allocated(false)
{
}

Image::~Image()
{
  if (m_data != nullptr && m_allocated) {
    stbi_image_free(m_data);
  }
}

Image::Image(Image&& other) noexcept
    : m_data(std::exchange(other.m_data, nullptr)),
      m_width(other.width()),
      m_height(other.height()),
      m_channels(other.channels()),
      m_allocated(false)
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

void Image::load(const std::filesystem::path& path, bool flip_vertically)
{
  stbi_set_flip_vertically_on_load(flip_vertically);
  m_data = stbi_load(path.string().c_str(), &m_width, &m_height, &m_channels, 0);
  m_allocated = true;
}

void Image::load_from_memory(const unsigned char* buffer, int len)
{
  m_data = stbi_load_from_memory(buffer, len, &m_width, &m_height, &m_channels, 3);
  m_channels = 3;
  m_allocated = false;
}

void Image::load_from_array(unsigned char* buffer, int width, int height, int channels)
{
  m_data = buffer;
  m_width = width;
  m_height = height;
  m_channels = channels;
  m_allocated = false;
}

bool Image::write_png(const std::filesystem::path& path) const
{
  return stbi_write_png(path.string().c_str(), m_width, m_height, m_channels, m_data, m_width * m_channels) == 1;
}

glm::u8vec4 Image::pixel(int x, int y) const
{
  assert(is_loaded());
  assert(0 <= x && x <= m_width);
  assert(0 <= y && y <= m_height);
  int i = (y * m_width + x) * 3;
  return glm::u8vec4(m_data[i + 0], m_data[i + 1], m_data[i + 2], 255U);
}

glm::u8vec4 Image::sample(const glm::vec2& uv, Sampling algorithm) const
{
  switch (algorithm) {
    // nearest neighbour
    case gfx::Image::NEAREST: {
      int x = static_cast<int>(uv.x * (m_width - 1));
      int y = static_cast<int>(uv.y * (m_height - 1));
      return pixel(x, y);
    }
    // bilinear interpolation
    case gfx::Image::LINEAR: {
      float x = uv.x * (m_width - 1);
      float y = uv.y * (m_height - 1);

      int x0 = static_cast<int>(x);
      int y0 = static_cast<int>(y);
      float x_frac = x - x0;
      float y_frac = y - x0;

      glm::u8vec4 top_left = pixel(x0, y0);
      glm::u8vec4 top_right = pixel(x0 + 1, y0);
      glm::u8vec4 bottom_left = pixel(x0, y0 + 1);
      glm::u8vec4 bottom_right = pixel(x0 + 1, y0 + 1);

      glm::u8vec4 top_interp = glm::mix(top_left, top_right, x_frac);
      glm::u8vec4 bottom_interp = glm::mix(bottom_left, bottom_right, x_frac);

      return glm::mix(top_interp, bottom_interp, y_frac);
    }
    default:
      assert(false);
      return glm::u8vec4();
  }
}

}  // namespace gfx