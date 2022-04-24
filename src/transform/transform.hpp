#ifndef TRANSFORM_HPP_
#define TRANSFORM_HPP_

#include "../matrix.hpp"

class Transform {
  public:
    virtual Matrix4 matrix() const = 0;
};

#endif
