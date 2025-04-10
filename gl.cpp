#include "gl.h"

#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>

#define STB_INCLUDE_LINE_NONE
#define STB_INCLUDE_IMPLEMENTATION
#include "stb/stb_include.h"

void check_gl_error(const char* stmt, const char* fname, int line)
{
  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    printf("OpenGL error %d, at %s:%i - for %s\n", err, fname, line, stmt);
  }
}

namespace gfx
{

namespace gl
{

Object::Object() noexcept : m_id(0) {}

Object::Object(Object&& rhs) noexcept : Object() { std::swap(m_id, rhs.m_id); }

Object& Object::operator=(Object&& rhs) noexcept
{
  if (this != &rhs) {
    std::swap(m_id, rhs.m_id);
  }

  return *this;
}


ShaderProgram::ShaderProgram(const std::filesystem::path& compute_shader_path)
    : ShaderProgram(compute_shader_path.string())
{
}

ShaderProgram::ShaderProgram(const std::filesystem::path& vertex_shader_path,
                             const std::filesystem::path& fragment_shader_path)
    : ShaderProgram(vertex_shader_path.string(), fragment_shader_path.string())
{
}

ShaderProgram::ShaderProgram(const std::string& compute_shader_path) : m_compute_shader_path(compute_shader_path)

{
  load_cs();
}

ShaderProgram::ShaderProgram(const std::string& vertex_shader_path, const std::string& fragment_shader_path)
    : m_vertex_shader_path(vertex_shader_path), m_fragment_shader_path(fragment_shader_path)
{
  load();
}

ShaderProgram::~ShaderProgram() { glDeleteProgram(m_id); }

void ShaderProgram::bind() const { glUseProgram(m_id); }

void ShaderProgram::unbind() const { glUseProgram(0); }

bool ShaderProgram::load()
{
  if (m_vertex_shader_path.empty() || m_fragment_shader_path.empty()) {
    return false;
  }

  char* vertex_shader_source = read_from_file_and_handle_includes(m_vertex_shader_path);

  if (!vertex_shader_source) {
    return false;
  }

  char* fragment_shader_source = read_from_file_and_handle_includes(m_fragment_shader_path);

  if (!fragment_shader_source) {
    return false;
  }

  GLuint vertex_shader = create_shader(GL_VERTEX_SHADER, vertex_shader_source);
  GLuint fragment_shader = create_shader(GL_FRAGMENT_SHADER, fragment_shader_source);

  free(vertex_shader_source);
  free(fragment_shader_source);

  GLuint id = create_program(vertex_shader, fragment_shader);

  if (id == 0) {
    return false;
  }

  // if there is already a shader, delete old shader
  if (m_id != 0) {
    glDeleteProgram(m_id);
  }

  m_id = id;
  return true;
}

bool ShaderProgram::load_cs()
{
  if (m_compute_shader_path.empty()) {
    return false;
  }

  char* shader_source = read_from_file_and_handle_includes(m_compute_shader_path);

  if (!shader_source) {
    return false;
  }

  GLuint compute_shader = create_shader(GL_COMPUTE_SHADER, shader_source);

  free(shader_source);

  int loaded;
  char log[512];

  GLuint id = glCreateProgram();
  glAttachShader(id, compute_shader);
  glLinkProgram(id);
  glGetProgramiv(id, GL_LINK_STATUS, &loaded);
  if (!loaded) {
    glGetProgramInfoLog(m_id, 512, NULL, log);
    std::cerr << "Error:\n" << log;
    id = 0;
  }

  glDeleteShader(compute_shader);

  if (id == 0) {
    return false;
  }

  // if there is already a shader, delete old shader
  if (m_id != 0) {
    glDeleteProgram(m_id);
  }

  m_id = id;
  return true;
}

GLuint ShaderProgram::create_shader(GLenum shader_type, const char* source)
{
  int loaded;
  char log[512];

  GLuint shader = glCreateShader(shader_type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &loaded);

  if (!loaded) {
    glGetShaderInfoLog(shader, 512, NULL, log);
    std::cerr << source << std::endl;
    std::cerr << "Error:\n" << log;
    return 0;
  }

  return shader;
}

GLuint ShaderProgram::create_program(GLuint s0, GLuint s1)
{
  if (s0 == 0 || s1 == 0) {
    return 0;
  }

  int loaded;
  char log[512];

  GLuint id = glCreateProgram();
  glAttachShader(id, s0);
  glAttachShader(id, s1);
  glLinkProgram(id);
  glGetProgramiv(id, GL_LINK_STATUS, &loaded);
  if (!loaded) {
    glGetProgramInfoLog(m_id, 512, NULL, log);
    std::cerr << "Error:\n" << log;
    id = 0;
  }

  glDeleteShader(s0);
  glDeleteShader(s1);

  return id;
}

void ShaderProgram::set_uniform(const std::string& name, GLint value) const
{
  glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void ShaderProgram::set_uniform(const std::string& name, GLuint value) const
{
  glUniform1ui(glGetUniformLocation(m_id, name.c_str()), value);
}

void ShaderProgram::set_uniform(const std::string& name, GLfloat value) const
{
  glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

void ShaderProgram::set_uniform(const std::string& name, const glm::vec2& value) const
{
  glUniform2fv(glGetUniformLocation(m_id, name.c_str()), 1, glm::value_ptr(value));
}

void ShaderProgram::set_uniform(const std::string& name, const glm::vec3& value) const
{
  glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, glm::value_ptr(value));
}

void ShaderProgram::set_uniform(const std::string& name, const glm::vec4& value) const
{
  glUniform4fv(glGetUniformLocation(m_id, name.c_str()), 1, glm::value_ptr(value));
}

void ShaderProgram::set_uniform(const std::string& name, const glm::mat3& value) const
{
  glUniformMatrix3fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::set_uniform(const std::string& name, const glm::mat4& value) const
{
  glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

char* ShaderProgram::read_from_file_and_handle_includes(const std::filesystem::path& path)
{
  auto shader_dir = path.parent_path();

  char error[512] = "";
  char* src = stb_include_file(path.string().c_str(), "", shader_dir.string().c_str(), error);

  std::string error_string = std::string(error);

  if (!error_string.empty()) {
    std::cerr << error << std::endl;
  }

  return src;
}

std::string ShaderProgram::from_file(const std::filesystem::path& path)
{
  std::ifstream file(path);
  if (!file.is_open()) {
    std::cerr << "Failed to open " << path.string() << std::endl;
    return std::string();
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

Texture::Texture(const Image& image) : Texture(image, {}) {}

Texture::Texture(const Image& image, const Params& params) : Texture()
{
  glBindTexture(GL_TEXTURE_2D, m_id);

  set_parameter(GL_TEXTURE_WRAP_S, params.wrap);
  set_parameter(GL_TEXTURE_WRAP_T, params.wrap);
  set_parameter(GL_TEXTURE_MIN_FILTER, params.min_filter);
  set_parameter(GL_TEXTURE_MAG_FILTER, params.mag_filter);

  glTexImage2D(GL_TEXTURE_2D, 0, image.format(), image.width(), image.height(), 0, image.format(), GL_UNSIGNED_BYTE,
               image.data());
  glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::bind(GLuint active_texture) const
{
  glActiveTexture(GL_TEXTURE0 + active_texture);
  glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture::bind() const { glBindTexture(GL_TEXTURE_2D, m_id); }

void Texture::unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

void Texture::set_parameter(GLenum pname, GLint param) { glTexParameteri(GL_TEXTURE_2D, pname, param); }

void Texture::set_parameter(GLenum pname, GLfloat param) { glTexParameterf(GL_TEXTURE_2D, pname, param); }

void Texture::set_parameter(GLenum pname, const GLfloat* param) { glTexParameterfv(GL_TEXTURE_2D, pname, param); }

void Texture::set_image(const Image& image)
{
  glTexImage2D(GL_TEXTURE_2D, 0, image.format(), image.width(), image.height(), 0, image.format(), GL_UNSIGNED_BYTE,
               image.data());
}

void Texture::generate_mipmap() { glGenerateMipmap(GL_TEXTURE_2D); }

std::unique_ptr<Texture> Texture::load(const std::string& path, const Params& params)
{
  Image image;
  image.load(path, params.flip_vertically);
  if (!image.is_valid()) return nullptr;
  return std::make_unique<Texture>();
}

std::unique_ptr<Texture> Texture::load(const std::string& path) { return Texture::load(path, {}); }

#if 0
std::unique_ptr<CubemapTexture> CubemapTexture::load(const std::vector<std::string>& paths)
{
  auto texture = std::make_unique<CubemapTexture>();

  texture->bind();

  for (int i = 0; i < 6; i++) {
    Image image;
    image.load(paths[i]);

    if (!image.is_valid()) {
      std::cerr << "Failed to load " << paths[i] << std::endl;
      return nullptr;
    }

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, image.format(), image.width(), image.height(), 0,
                 image.format(), GL_UNSIGNED_BYTE, image.data());
  }

  return texture;
}
#endif

}  // namespace gl
}  // namespace gfx