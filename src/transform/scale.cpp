#include "scale.hpp"

#include <optional>

class Scale::Impl {
  public:
    Impl(Vector3 offset);
    Matrix4 matrix() const;
    void change(const Vector3& delta);

  private:
    Vector3 scale;
    mutable std::optional<Matrix4> cached;
};

Scale::Scale(Vector3 offset) : impl(std::make_unique<Impl>(offset)) {}
Scale::Impl::Impl(Vector3 offset) : scale(offset) {}

Scale::~Scale() = default;

Matrix4 Scale::matrix() const { return impl->matrix(); }
Matrix4 Scale::Impl::matrix() const {
    if (cached.has_value() == false) {
        const auto [x, y, z] = scale;
        cached = Matrix4{x, 0, 0, 0, //
                         0, y, 0, 0, //
                         0, 0, z, 0, //
                         0, 0, 0, 1};
    }
    return *cached;
}

void Scale::change(const Vector3& delta) { impl->change(delta); }
void Scale::Impl::change(const Vector3& delta) {
    const auto [x, y, z] = delta;
    if (x == 0.0 && y == 0.0 && z == 0.0) {
        return;
    }

    scale += delta;
    cached = std::nullopt;
}

Scale::Scale(Scale&&) = default;
Scale& Scale::operator=(Scale&&) = default;
