#ifndef TRANSFORM_HPP_
#define TRANSFORM_HPP_

#include "../matrix.hpp"

// Abstract class for types producing transform matrices.
class Transform {
  public:
    virtual Matrix4 matrix() const = 0;
    virtual ~Transform() = default;
};

#endif
