#include "image.h"

#include <iostream>
#include <utility>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

constexpr int DEFAULT_IMAGE_CHANNELS = 3;

namespace gfx
{

Image::Image() noexcept : m_data(nullptr), m_width(0), m_height(0), m_channels(0) {}

Image::Image(const unsigned char* buffer, int len) noexcept : Image()
{
  int channels_in_file;
  m_channels = DEFAULT_IMAGE_CHANNELS;
  m_data = stbi_load_from_memory(buffer, len, &m_width, &m_height, &channels_in_file, m_channels);
}

Image::Image(Image&& other) noexcept : Image() { *this = std::move(other); }

Image& Image::operator=(Image&& other) noexcept
{
  if (this != &other) {
    cleanup();
    m_data = other.m_data;
    m_width = other.m_width;
    m_height = other.m_height;
    m_channels = other.m_channels;
    other.reset();
  }
  return *this;
}

Image::~Image() noexcept { cleanup(); }

void Image::cleanup()
{
  if (m_data) {
    stbi_image_free(m_data);
  }
  reset();
}

void Image::reset()
{
  m_data = nullptr;
  m_width = 0;
  m_height = 0;
  m_channels = 0;
}

unsigned char* Image::data() const { return m_data; }

int Image::width() const { return m_width; }

int Image::height() const { return m_height; }

int Image::channels() const { return m_channels; }

bool Image::is_valid() const { return (m_data != nullptr) && (0 < m_width) && (0 < m_height) && (0 < m_channels); }

Image::Format Image::format() const
{
  assert(1 <= m_channels && m_channels <= 4);
  static Format formats[] = {RED, RG, RGB, RGBA};
  return formats[m_channels - 1];
}

bool Image::load(const std::filesystem::path& path, bool flip_vertically)
{
  int channels_in_file;
  m_channels = DEFAULT_IMAGE_CHANNELS;
  stbi_set_flip_vertically_on_load(flip_vertically);
  m_data = stbi_load(path.string().c_str(), &m_width, &m_height, &channels_in_file, m_channels);
  return is_valid();
}

bool Image::write_png(const std::filesystem::path& path) const
{
  if (!is_valid()) {
    return false;
  }
  return stbi_write_png(path.string().c_str(), m_width, m_height, m_channels, m_data, m_width * m_channels) == 1;
}

glm::u8vec4 Image::pixel(int x, int y) const
{
  if (is_valid()) {
    assert(0 <= x && x <= m_width);
    assert(0 <= y && y <= m_height);
    int i = (y * m_width + x) * 3;
    return glm::u8vec4(m_data[i + 0], m_data[i + 1], m_data[i + 2], 255U);
  } else {
    return glm::u8vec4(0);
  }
}

glm::u8vec4 Image::sample(const glm::vec2& uv, Sampling algorithm) const
{
  if (!is_valid()) {
    return glm::u8vec4(0);
  }

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