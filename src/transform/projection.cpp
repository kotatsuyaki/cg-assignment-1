#include "projection.hpp"

#include <algorithm>
#include <array>
#include <memory>
#include <optional>
#include <stdexcept>

#include "../mathutils.hpp"

class Projection::Impl {
  public:
    Impl(float near_clip, float far_clip, float fovy, float aspect, float left, float right,
         float top, float bottom)
        : near_clip(near_clip), far_clip(far_clip), fovy(fovy), aspect(aspect), left(left),
          right(right), top(top), bottom(bottom), cached(std::nullopt), mode(Mode::Perspective) {}

    Matrix4 matrix() const;
    void set_aspect(float aspect);
    void set_mode(Mode mode);

  private:
    float near_clip;
    float far_clip;
    float fovy;
    float aspect;
    float left;
    float right;
    float top;
    float bottom;

    Mode mode;

    mutable std::optional<Matrix4> cached;
    Matrix4 matrix_orthogonal() const;
    Matrix4 matrix_perspective() const;
};

Projection::Projection(float near_clip, float far_clip, float fovy, float aspect, float left,
                       float right, float top, float bottom)
    : impl(std::make_unique<Impl>(near_clip, far_clip, fovy, aspect, left, right, top, bottom)) {}
Projection::~Projection() = default;
Matrix4 Projection::matrix() const { return impl->matrix(); }

Matrix4 Projection::Impl::matrix() const {
    if (cached.has_value()) {
        return *cached;
    } else {
        switch (mode) {
        case Mode::Perspective:
            cached = matrix_perspective();
            break;
        case Mode::Orthogonal:
            cached = matrix_orthogonal();
            break;
        }
        return *cached;
    }
}

Matrix4 Projection::Impl::matrix_perspective() const {
    const float tan_term = std::tan(deg2rad(fovy) * 0.5f);

    const float x_scale = 1.0f / (tan_term * aspect);
    const float y_scale = 1.0f / tan_term;
    const float z_scale_a = (near_clip + far_clip) / (near_clip - far_clip);
    const float z_scale_b = 2 * near_clip * far_clip / (near_clip - far_clip);

    return Matrix4{x_scale, 0,       0,         0,         //
                   0,       y_scale, 0,         0,         //
                   0,       0,       z_scale_a, z_scale_b, //
                   0,       0,       -1,        0};
}

Matrix4 Projection::Impl::matrix_orthogonal() const {
    const float x_scale = 2.0f / ((right - left) * aspect);
    const float y_scale = 2.0f / (top - bottom);
    const float z_scale = 2.0f / (near_clip - far_clip);

    const float x_trans = -1.0f * (right + left) / (right - left);
    const float y_trans = -1.0f * (top + bottom) / (top - bottom);
    const float z_trans = -1.0f * (far_clip + near_clip) / (far_clip - near_clip);

    return Matrix4{x_scale, 0,       0,       x_trans, //
                   0,       y_scale, 0,       y_trans, //
                   0,       0,       z_scale, z_trans, //
                   0,       0,       0,       1};
}

void Projection::set_aspect(float aspect) { impl->set_aspect(aspect); }
void Projection::Impl::set_aspect(float aspect) {
    this->aspect = aspect;
    cached = std::nullopt;
}

void Projection::set_mode(Mode mode) { impl->set_mode(mode); }
void Projection::Impl::set_mode(Mode mode) {
    this->mode = mode;
    cached = std::nullopt;
}

struct ProjectionBuilder::Impl {
    std::optional<float> near_clip;
    std::optional<float> far_clip;
    std::optional<float> fovy;
    std::optional<float> aspect;
    std::optional<float> left;
    std::optional<float> right;
    std::optional<float> top;
    std::optional<float> bottom;
};

ProjectionBuilder::ProjectionBuilder() { impl = std::make_unique<Impl>(); }
ProjectionBuilder::~ProjectionBuilder() = default;

ProjectionBuilder& ProjectionBuilder::with_near_clip(float value) {
    impl->near_clip = value;
    return *this;
}
ProjectionBuilder& ProjectionBuilder::with_far_clip(float value) {
    impl->far_clip = value;
    return *this;
}
ProjectionBuilder& ProjectionBuilder::with_fovy(float value) {
    impl->fovy = value;
    return *this;
}
ProjectionBuilder& ProjectionBuilder::with_aspect(float value) {
    impl->aspect = value;
    return *this;
}
ProjectionBuilder& ProjectionBuilder::with_left(float value) {
    impl->left = value;
    return *this;
}
ProjectionBuilder& ProjectionBuilder::with_right(float value) {
    impl->right = value;
    return *this;
}
ProjectionBuilder& ProjectionBuilder::with_top(float value) {
    impl->top = value;
    return *this;
}
ProjectionBuilder& ProjectionBuilder::with_bottom(float value) {
    impl->bottom = value;
    return *this;
}

Projection ProjectionBuilder::build() const {
    std::array<std::optional<float>, 8> opts = {impl->near_clip, impl->far_clip, impl->fovy,
                                                impl->aspect,    impl->left,     impl->right,
                                                impl->top,       impl->bottom};
    const bool has_unset = std::any_of(opts.begin(), opts.end(), [](std::optional<float> opt) {
        return opt.has_value() == false;
    });
    if (has_unset) {
        throw std::runtime_error("Building Projection failed due to unset field(s)");
    }

    return Projection{*impl->near_clip, *impl->far_clip, *impl->fovy, *impl->aspect,
                      *impl->left,      *impl->right,    *impl->top,  *impl->bottom};
}
