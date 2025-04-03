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

Image::Image(int width, int height, int channels) noexcept
    : m_data(nullptr), m_width(width), m_height(height), m_channels(channels)
{
  int len = sizeof(*m_data) * width * height * channels;
  m_data = (unsigned char*)STBI_MALLOC(len);
  if (m_data) {
    std::memset(m_data, 0x0, len);
  }
}

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

void Image::set_pixel(int x, int y, const unsigned char* pixel)
{
  if (is_valid() && (0 <= x && x < m_width) && (0 <= y && y < m_height)) {
    int i = (y * m_width + x) * m_channels;
    for (int c = 0; c < m_channels; c++) {
      m_data[i + c] = pixel[c];
    }
  }
}

glm::u8vec4 Image::pixel(int x, int y) const
{
  glm::u8vec4 pixel(0);

  if (is_valid() && (0 <= x && x < m_width) && (0 <= y && y < m_height)) {
    int i = (y * m_width + x) * m_channels;
    for (int c = 0; c < glm::min(m_channels, 4); c++) {
      pixel[c] = m_data[i + c];
    }
  }

  return pixel;
}

glm::u8vec4 Image::sample(const glm::vec2& uv, Sampling algorithm) const
{
  if (!is_valid() || !(0.f <= uv.x && uv.x <= 1.0f) || !(0.f <= uv.y && uv.y <= 1.0f)) {
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

      int x0 = (int)x;
      int y0 = (int)y;
      float x_frac = x - x0;
      float y_frac = y - x0;

      glm::u8vec4 tl = pixel(x0 + 0, y0 + 0);
      glm::u8vec4 tr = pixel(x0 + 1, y0 + 0);
      glm::u8vec4 bl = pixel(x0 + 0, y0 + 1);
      glm::u8vec4 br = pixel(x0 + 1, y0 + 1);

      glm::u8vec4 t = glm::mix(tl, tr, x_frac);
      glm::u8vec4 b = glm::mix(bl, br, x_frac);

      return glm::mix(t, b, y_frac);
    }
    default:
      assert(false);
      return glm::u8vec4();
  }
}

}  // namespace gfx