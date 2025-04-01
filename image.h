
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

  Image()
       : m_data(nullptr)
       , m_width(0)
       , m_height(0)
       , m_channels(0)
   {
   }

   Image(int width, int height, int channels)
   {
      m_data = new unsigned char[width * height * channels]();
      m_width = width;
      m_height = height;
      m_channels = channels;
   }

   Image(const Image& other)           = delete;
   Image& operator=(const Image& that) = delete;

   Image(Image&& other)
       : Image()
   {
      *this = std::move(other);
   }

   Image& operator=(Image&& other) noexcept
   {
      if(this != &other)
      {
         cleanup();
         m_data     = other.m_data;
         m_width    = other.m_width;
         m_height   = other.m_height;
         m_channels = other.m_channels;
         other.reset();
      }
      return *this;
   }

   ~Image() noexcept
   {
      printf("%s\n", __PRETTY_FUNCTION__);
      cleanup();
   }

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


   void cleanup()
   {
      printf("%s\n", __PRETTY_FUNCTION__);
      if(m_data)
      {
         printf("%s delete %p\n", __PRETTY_FUNCTION__, (void*) m_data);
         delete[] m_data;
      }
      reset();
   }

   void reset()
   {
      printf("%s\n", __PRETTY_FUNCTION__);
      m_data     = nullptr;
      m_width    = 0;
      m_height   = 0;
      m_channels = 0;
   }
};
}  // namespace gfx
