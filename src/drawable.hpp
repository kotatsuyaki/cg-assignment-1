#ifndef DRAWABLE_HPP_
#define DRAWABLE_HPP_

#include <memory>

#include "shader.hpp"

class Drawable {
  public:
    virtual void draw(Shader& shader) const = 0;
    virtual ~Drawable() = default;
};

#endif
