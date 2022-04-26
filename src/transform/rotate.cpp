#include "rotate.hpp"

#include <cmath>
#include <optional>

#include "../mathutils.hpp"

using std::cos;
using std::sin;

class Rotate::Impl {
  public:
    Impl(Vector3 offset);
    Matrix4 matrix() const;
    void change(const Vector3& delta);

  private:
    Vector3 scale;
    mutable std::optional<Matrix4> cached;
};

namespace {
Matrix4 rotate_x(float deg) {
    const float rad = deg2rad(deg);

    return Matrix4{1, 0,        0,         0, //
                   0, cos(rad), -sin(rad), 0, //
                   0, sin(rad), cos(rad),  0, //
                   0, 0,        0,         1};
}

Matrix4 rotate_y(float deg) {
    const float rad = deg2rad(deg);

    return Matrix4{cos(rad),  0, sin(rad), 0, //
                   0,         1, 0,        0, //
                   -sin(rad), 0, cos(rad), 0, //
                   0,         0, 0,        1};
}

Matrix4 rotate_z(float deg) {
    const float rad = deg2rad(deg);

    return Matrix4{cos(rad), -sin(rad), 0, 0, //
                   sin(rad), cos(rad),  0, 0, //
                   0,        0,         1, 0, //
                   0,        0,         0, 1};
}
} // namespace

Rotate::Rotate(Vector3 offset) : impl(std::make_unique<Impl>(offset)) {}
Rotate::Impl::Impl(Vector3 offset) : scale(offset) {}

Rotate::~Rotate() = default;

Matrix4 Rotate::matrix() const { return impl->matrix(); }
Matrix4 Rotate::Impl::matrix() const {
    if (cached.has_value() == false) {
        const auto [x, y, z] = scale;
        cached = rotate_x(x) * rotate_y(y) * rotate_z(z);
    }
    return *cached;
}

void Rotate::change(const Vector3& delta) { impl->change(delta); }
void Rotate::Impl::change(const Vector3& delta) {
    const auto [x, y, z] = delta;
    if (x == 0.0 && y == 0.0 && z == 0.0) {
        return;
    }

    scale += delta;
    cached = std::nullopt;
}

Rotate::Rotate(Rotate&&) = default;
Rotate& Rotate::operator=(Rotate&&) = default;
