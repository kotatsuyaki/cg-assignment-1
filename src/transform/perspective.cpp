#include "perspective.hpp"

#include <algorithm>
#include <array>
#include <memory>
#include <optional>

#include "../mathutils.hpp"

struct Perspective::PerspectiveImpl {
    float near_clip;
    float far_clip;
    float fovy;
    float aspect;
    float left;
    float right;
    float top;
    float bottom;

    PerspectiveImpl(float near_clip, float far_clip, float fovy, float aspect, float left,
                    float right, float top, float bottom)
        : near_clip(near_clip), far_clip(far_clip), fovy(fovy), aspect(aspect), left(left),
          right(right), top(top), bottom(bottom) {}
    Matrix4 matrix() const;
};

Perspective::Perspective(float near_clip, float far_clip, float fovy, float aspect, float left,
                         float right, float top, float bottom)
    : impl(std::make_unique<PerspectiveImpl>(near_clip, far_clip, fovy, aspect, left, right, top,
                                             bottom)) {}
Perspective::~Perspective() = default;
Matrix4 Perspective::matrix() const { return impl->matrix(); }

Matrix4 Perspective::PerspectiveImpl::matrix() const {
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

struct PerspectiveBuilder::PerspectiveBuilderImpl {
    std::optional<float> near_clip;
    std::optional<float> far_clip;
    std::optional<float> fovy;
    std::optional<float> aspect;
    std::optional<float> left;
    std::optional<float> right;
    std::optional<float> top;
    std::optional<float> bottom;
};

PerspectiveBuilder::PerspectiveBuilder() { impl = std::make_unique<PerspectiveBuilderImpl>(); }
PerspectiveBuilder::~PerspectiveBuilder() = default;

PerspectiveBuilder& PerspectiveBuilder::with_near_clip(float value) {
    impl->near_clip = value;
    return *this;
}
PerspectiveBuilder& PerspectiveBuilder::with_far_clip(float value) {
    impl->far_clip = value;
    return *this;
}
PerspectiveBuilder& PerspectiveBuilder::with_fovy(float value) {
    impl->fovy = value;
    return *this;
}
PerspectiveBuilder& PerspectiveBuilder::with_aspect(float value) {
    impl->aspect = value;
    return *this;
}
PerspectiveBuilder& PerspectiveBuilder::with_left(float value) {
    impl->left = value;
    return *this;
}
PerspectiveBuilder& PerspectiveBuilder::with_right(float value) {
    impl->right = value;
    return *this;
}
PerspectiveBuilder& PerspectiveBuilder::with_top(float value) {
    impl->top = value;
    return *this;
}
PerspectiveBuilder& PerspectiveBuilder::with_bottom(float value) {
    impl->bottom = value;
    return *this;
}

Perspective PerspectiveBuilder::build() const {
    std::array<std::optional<float>, 8> opts = {impl->near_clip, impl->far_clip, impl->fovy,
                                                impl->aspect,    impl->left,     impl->right,
                                                impl->top,       impl->bottom};
    const bool has_unset = std::any_of(opts.begin(), opts.end(), [](std::optional<float> opt) {
        return opt.has_value() == false;
    });
    if (has_unset) {
        throw std::runtime_error("Building Perspective failed due to unset field(s)");
    }

    return Perspective{*impl->near_clip, *impl->far_clip, *impl->fovy, *impl->aspect,
                       *impl->left,      *impl->right,    *impl->top,  *impl->bottom};
}
