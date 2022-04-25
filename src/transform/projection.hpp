#ifndef PROJECTION_HPP_
#define PROJECTION_HPP_

#include "transform.hpp"

#include <memory>

// Perspective and orthogonal projections.
// Use ProjectionBuilder to build an instance of it.
class Projection final : public Transform {
  public:
    ~Projection();
    virtual Matrix4 matrix() const override;

    // Updates the aspect ratio of viewport, calculated as width / height.
    void set_aspect(float aspect);

    enum class Mode { Perspective, Orthogonal };
    // Updates the projection mode.
    void set_mode(Mode mode);

  private:
    class Impl;
    std::unique_ptr<Impl> impl;

    explicit Projection(float near_clip, float far_clip, float fovy, float aspect, float left,
                        float right, float top, float bottom);
    friend class ProjectionBuilder;
};

class ProjectionBuilder final {
  public:
    ProjectionBuilder();
    ~ProjectionBuilder();

    ProjectionBuilder& with_near_clip(float value);
    ProjectionBuilder& with_far_clip(float value);
    ProjectionBuilder& with_fovy(float value);
    ProjectionBuilder& with_aspect(float value);
    ProjectionBuilder& with_left(float value);
    ProjectionBuilder& with_right(float value);
    ProjectionBuilder& with_top(float value);
    ProjectionBuilder& with_bottom(float value);

    // Builds an instance of Projection.
    // Throws if there are values not assigned yet.
    Projection build() const;

  private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

#endif
