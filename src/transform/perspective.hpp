#ifndef PERSPECTIVE_HPP_
#define PERSPECTIVE_HPP_

#include "transform.hpp"

#include <memory>

// Perspective projection transform.
// Use PerspectiveBuilder to build an instance of it.
class Perspective final : public Transform {
  public:
    ~Perspective();
    virtual Matrix4 matrix() const override;

  private:
    struct PerspectiveImpl;
    std::unique_ptr<PerspectiveImpl> impl;

    explicit Perspective(float near_clip, float far_clip, float fovy, float aspect, float left,
                         float right, float top, float bottom);
    friend class PerspectiveBuilder;
};

class PerspectiveBuilder final {
  public:
    PerspectiveBuilder();
    ~PerspectiveBuilder();

    PerspectiveBuilder& with_near_clip(float value);
    PerspectiveBuilder& with_far_clip(float value);
    PerspectiveBuilder& with_fovy(float value);
    PerspectiveBuilder& with_aspect(float value);
    PerspectiveBuilder& with_left(float value);
    PerspectiveBuilder& with_right(float value);
    PerspectiveBuilder& with_top(float value);
    PerspectiveBuilder& with_bottom(float value);

    // Builds an instance of Perspective.
    // Throws if there are values not assigned yet.
    Perspective build() const;

  private:
    struct PerspectiveBuilderImpl;
    std::unique_ptr<PerspectiveBuilderImpl> impl;
};

#endif
