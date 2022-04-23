#ifndef DRAWABLE_HPP_
#define DRAWABLE_HPP_

#include <memory>

class Drawable {
  public:
    virtual void draw() const = 0;
};

#endif
