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

ShaderProgram::ShaderProgram(const std::filesystem::path& vertex_shader_path,
                             const std::filesystem::path& fragment_shader_path)
    : ShaderProgram(vertex_shader_path.string(), fragment_shader_path.string())
{
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

Texture::Texture(const Image& image, const Params& params) : Texture(GL_TEXTURE_2D)
{
  glBindTexture(target, m_id);

  set_parameter(GL_TEXTURE_WRAP_S, params.wrap);
  set_parameter(GL_TEXTURE_WRAP_T, params.wrap);
  set_parameter(GL_TEXTURE_MIN_FILTER, params.min_filter);
  set_parameter(GL_TEXTURE_MAG_FILTER, params.mag_filter);

  glTexImage2D(target, 0, image.format(), image.width(), image.height(), 0, image.format(), GL_UNSIGNED_BYTE,
               image.data());
  glGenerateMipmap(target);
}

void Texture::bind(GLuint active_texture) const
{
  glActiveTexture(GL_TEXTURE0 + active_texture);
  glBindTexture(target, m_id);
}

void Texture::bind() const { glBindTexture(target, m_id); }

void Texture::unbind() const { glBindTexture(target, 0); }

void Texture::set_parameter(GLenum pname, GLint param) { glTexParameteri(target, pname, param); }

void Texture::set_parameter(GLenum pname, GLfloat param) { glTexParameterf(target, pname, param); }

void Texture::set_parameter(GLenum pname, const GLfloat* param) { glTexParameterfv(target, pname, param); }

void Texture::set_image(const Image& image)
{
  glTexImage2D(target, 0, image.format(), image.width(), image.height(), 0, image.format(), GL_UNSIGNED_BYTE,
               image.data());
}

void Texture::generate_mipmap() { glGenerateMipmap(target); }

std::unique_ptr<Texture> Texture::load(const std::string& path, const Params& params)
{
  Image image;
  image.load(path, params.flip_vertically);
  if (!image.is_valid()) return nullptr;
  return std::make_unique<Texture>(image, params);
}

std::unique_ptr<Texture> Texture::load(const std::string& path) { return Texture::load(path, {}); }

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

}  // namespace gl
}  // namespace gfx