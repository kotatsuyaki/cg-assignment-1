#include "mvp.hpp"
#include "projection.hpp"
#include "rotate.hpp"
#include "scale.hpp"
#include "translate.hpp"
#include "viewer.hpp"

#include <array>
#include <memory>
#include <optional>
#include <utility>

class Mvp::Impl {
  public:
    Impl(int width, int height);

    Matrix4 matrix() const;
    void set_viewport_size(int width, int height);
    void set_project_mode(Projection::Mode mode);
    void debug_print() const;

    void update_translation(Vector3 delta);
    void update_rotation(Vector3 delta);
    void update_scaling(Vector3 delta);

  private:
    Projection proj;
    Viewer viewer;

    Translate trans;
    Rotate rotate;
    Scale scale;

    mutable std::optional<Matrix4> cached;
};

Mvp::Impl::Impl(int width, int height)
    : proj(ProjectionBuilder()
               .with_left(-1)
               .with_right(1)
               .with_bottom(-1)
               .with_top(1)
               .with_near_clip(0.001f)
               .with_far_clip(100)
               .with_fovy(80)
               .with_aspect(static_cast<float>(width) / static_cast<float>(height))
               .build()),
      viewer({0.0, 0.0, 2.0}, {0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}), trans({0, 0, 0}),
      rotate({0, 0, 0}), scale({1, 1, 1}), cached(std::nullopt) {}

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

void Mvp::debug_print() const { impl->debug_print(); }
void Mvp::Impl::debug_print() const {
    std::array<std::pair<const char*, const Transform&>, 2> pairs{
        {{"Viewing matrix", viewer}, {"Projection matrix", proj}}};
    for (const auto& [name, transform] : pairs) {
        std::cout << name << ":\n" << transform.matrix() << "\n";
    }
}

void Mvp::update_translation(Vector3 delta) { impl->update_translation(delta); }
void Mvp::Impl::update_translation(Vector3 delta) {
    trans.change(delta);
    cached = std::nullopt;
}

void Mvp::update_rotation(Vector3 delta) { impl->update_rotation(delta); }
void Mvp::Impl::update_rotation(Vector3 delta) {
    rotate.change(delta);
    cached = std::nullopt;
}

void Mvp::update_scaling(Vector3 delta) { impl->update_scaling(delta); }
void Mvp::Impl::update_scaling(Vector3 delta) {
    rotate.change(delta);
    cached = std::nullopt;
}

Matrix4 Mvp::Impl::matrix() const {
    if (cached.has_value() == false) {
        cached =
            proj.matrix() * viewer.matrix() * trans.matrix() * rotate.matrix() * scale.matrix();
    }
    return *cached;
}
Matrix4 Mvp::matrix() const { return impl->matrix(); }
