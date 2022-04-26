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
    Matrix4 model_matrix() const;
    Matrix4 view_project_matrix() const;

    void set_viewport_size(int width, int height);
    void set_project_mode(Projection::Mode mode);
    void debug_print() const;

    void update_translation(Vector3 delta);
    void update_rotation(Vector3 delta);
    void update_scaling(Vector3 delta);

    void update_eyepos(Vector3 delta);
    void update_center(Vector3 delta);
    void update_up(Vector3 delta);

  private:
    Projection proj;
    Viewer viewer;

    Translate trans;
    Rotate rotate;
    Scale scale;

    mutable std::optional<Matrix4> cached;
    mutable std::optional<Matrix4> cached_vp;
    mutable std::optional<Matrix4> cached_trs;

    void inval_vp();
    void inval_trs();
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
      rotate({0, 0, 0}), scale({1, 1, 1}), cached(std::nullopt), cached_vp(std::nullopt),
      cached_trs(std::nullopt) {}

Mvp::Mvp(int width, int height) : impl(std::make_unique<Impl>(width, height)) {}
Mvp::~Mvp() = default;

void Mvp::Impl::set_viewport_size(int width, int height) {
    proj.set_aspect(static_cast<float>(width) / height);
    inval_vp();
}
void Mvp::set_viewport_size(int width, int height) { impl->set_viewport_size(width, height); }

void Mvp::Impl::set_project_mode(Projection::Mode mode) {
    proj.set_mode(mode);
    inval_vp();
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
    std::array<std::pair<const char*, const Transform&>, 5> pairs{{{"Viewing", viewer},
                                                                   {"Projection", proj},
                                                                   {"Translation", trans},
                                                                   {"Rotation", rotate},
                                                                   {"Scaling", scale}}};
    for (const auto& [name, transform] : pairs) {
        std::cout << name << " matrix:\n" << transform.matrix() << "\n";
    }
}

void Mvp::update_translation(Vector3 delta) { impl->update_translation(delta); }
void Mvp::Impl::update_translation(Vector3 delta) {
    trans.change(delta);
    inval_trs();
}

void Mvp::update_rotation(Vector3 delta) { impl->update_rotation(delta); }
void Mvp::Impl::update_rotation(Vector3 delta) {
    rotate.change(delta);
    inval_trs();
}

void Mvp::update_scaling(Vector3 delta) { impl->update_scaling(delta); }
void Mvp::Impl::update_scaling(Vector3 delta) {
    scale.change(delta);
    inval_trs();
}

void Mvp::update_eyepos(Vector3 delta) { impl->update_eyepos(delta); }
void Mvp::Impl::update_eyepos(Vector3 delta) {
    viewer.change_eyepos(delta);
    inval_vp();
}

void Mvp::update_center(Vector3 delta) { impl->update_center(delta); }
void Mvp::Impl::update_center(Vector3 delta) {
    viewer.change_center(delta);
    inval_vp();
}

void Mvp::update_up(Vector3 delta) { impl->update_up(delta); }
void Mvp::Impl::update_up(Vector3 delta) {
    viewer.change_up(delta);
    inval_vp();
}

Matrix4 Mvp::matrix() const { return impl->matrix(); }
Matrix4 Mvp::Impl::matrix() const {
    if (cached.has_value() == false) {
        cached = view_project_matrix() * model_matrix();
    }
    return *cached;
}

Matrix4 Mvp::model_matrix() const { return impl->model_matrix(); }
Matrix4 Mvp::Impl::model_matrix() const {
    if (cached_trs.has_value() == false) {
        cached = trans.matrix() * rotate.matrix() * scale.matrix();
    }
    return *cached;
}

Matrix4 Mvp::view_project_matrix() const { return impl->view_project_matrix(); }
Matrix4 Mvp::Impl::view_project_matrix() const {
    if (cached_vp.has_value() == false) {
        cached = proj.matrix() * viewer.matrix();
    }
    return *cached;
}

void Mvp::Impl::inval_vp() { cached = cached_vp = std::nullopt; }
void Mvp::Impl::inval_trs() { cached = cached_trs = std::nullopt; }
