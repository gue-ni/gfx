#pragma once

#include "gl.h"

#include <memory>

struct Material {
  std::shared_ptr<gl::Shader> shader = nullptr;
};
