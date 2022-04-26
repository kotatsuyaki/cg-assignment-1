#include "translate.hpp"

#include <optional>

class Translate::Impl {
  public:
    Impl(Vector3 offset);
    Matrix4 matrix() const;
    void move(const Vector3& delta);

  private:
    Vector3 offset;
    mutable std::optional<Matrix4> cached;
};

Translate::Translate(Vector3 offset) : impl(std::make_unique<Impl>(offset)) {}
Translate::Impl::Impl(Vector3 offset) : offset(offset) {}

Translate::~Translate() = default;

Matrix4 Translate::matrix() const { return impl->matrix(); }
Matrix4 Translate::Impl::matrix() const {
    if (cached.has_value() == false) {
        const auto [x, y, z] = offset;
        cached = Matrix4{1, 0, 0, x, //
                         0, 1, 0, y, //
                         0, 0, 1, z, //
                         0, 0, 0, 1};
    }
    return *cached;
}

void Translate::change(const Vector3& delta) { impl->move(delta); }
void Translate::Impl::move(const Vector3& delta) {
    const auto [x, y, z] = delta;
    if (x == 0.0 && y == 0.0 && z == 0.0) {
        return;
    }

    offset += delta;
    cached = std::nullopt;
}

Translate::Translate(Translate&&) = default;
Translate& Translate::operator=(Translate&&) = default;
