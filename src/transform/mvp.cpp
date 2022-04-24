#include "mvp.hpp"
#include "projection.hpp"
#include "viewer.hpp"

#include <memory>
#include <optional>

struct Mvp::Impl {
    Impl(int width, int height);

    Matrix4 matrix() const;
    void set_viewport_size(int width, int height);
    void set_project_mode(Projection::Mode mode);

    Projection proj;
    Viewer viewer;
    mutable std::optional<Matrix4> cached;
};

Mvp::Impl::Impl(int width, int height)
    : proj(ProjectionBuilder()
               .with_left(-1)
               .with_right(1)
               .with_bottom(-1)
               .with_top(1)
               .with_near_clip(0.001)
               .with_far_clip(100)
               .with_fovy(80)
               .with_aspect(static_cast<float>(width) / static_cast<float>(height))
               .build()),
      viewer({0.0, 0.0, 2.0}, {0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}), cached(std::nullopt) {}

Mvp::Mvp(int width, int height) : impl(std::make_unique<Impl>(width, height)) {}
Mvp::~Mvp() = default;

void Mvp::Impl::set_viewport_size(int width, int height) {
    proj.set_aspect(static_cast<float>(width) / height);
    cached = std::nullopt;
}
void Mvp::set_viewport_size(int width, int height) { impl->set_viewport_size(width, height); }

void Mvp::Impl::set_project_mode(Projection::Mode mode) {
    proj.set_mode(mode);
    cached = std::nullopt;
}
void Mvp::set_project_mode(ProjectMode mode) {
    Projection::Mode pmode;
    switch (mode) {
    case ProjectMode::Perspective:
        pmode = Projection::Mode::Perspective;
        break;
    case ProjectMode::Orthogonal:
        pmode = Projection::Mode::Orthogonal;
        break;
    }
    impl->set_project_mode(pmode);
}

Matrix4 Mvp::Impl::matrix() const {
    if (cached.has_value()) {
        return *cached;
    } else {
        auto mat = proj.matrix() * viewer.matrix();
        cached = mat;
        return mat;
    }
}
Matrix4 Mvp::matrix() const { return impl->matrix(); }
