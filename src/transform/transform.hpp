#ifndef TRANSFORM_HPP_
#define TRANSFORM_HPP_

#include "../matrix.hpp"

// Abstract class for types producing transform matrices.
class Transform {
  public:
    virtual Matrix4 matrix() const = 0;
    virtual ~Transform() = default;
};

// Abstract class for multi-stage transforms.
class StagedTransform : public Transform {
  public:
    virtual Matrix4 model_matrix() const = 0;
    virtual Matrix4 view_project_matrix() const = 0;
};

#endif
