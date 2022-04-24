#include "mvp.hpp"
#include "perspective.hpp"
#include "viewer.hpp"

#include <memory>

struct Mvp::Impl {
    Impl(int width, int height);

    Matrix4 matrix() const;

    Perspective perspective;
    Viewer viewer;
};

Mvp::Impl::Impl(int width, int height)
    : perspective(PerspectiveBuilder()
                      .with_left(-1)
                      .with_right(1)
                      .with_bottom(-1)
                      .with_top(1)
                      .with_near_clip(0.001)
                      .with_far_clip(100)
                      .with_fovy(80)
                      .with_aspect(static_cast<float>(width) / static_cast<float>(height))
                      .build()),
      viewer({0.0, 0.0, 2.0}, {0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}) {}

Matrix4 Mvp::Impl::matrix() const { return perspective.matrix() * viewer.matrix(); }
Matrix4 Mvp::matrix() const { return impl->matrix(); }

Mvp::Mvp(int width, int height) : impl(std::make_unique<Impl>(width, height)) {}
Mvp::~Mvp() = default;
