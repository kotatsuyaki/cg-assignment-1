#include "control.hpp"

namespace {
const Vector3 DIRECTION_SCALES{0.01, 0.01, 0.01};
const float ROTATION_SCALE = 5.0;
} // namespace

class MvpControl::Impl {
  public:
    Impl();
    void update(Mvp& mvp);
    void update_offset(Vector3 offset);
    void set_pressed(bool pressed);
    void set_mode(Mode mode);

  private:
    Mode mode;
    Vector3 accumulated;
    bool pressed;
};

MvpControl::MvpControl() : impl(std::make_unique<Impl>()) {}
MvpControl::Impl::Impl() : mode(Mode::TranslateModel), accumulated({0, 0, 0}), pressed(false) {}

MvpControl::~MvpControl() = default;

void MvpControl::update(Mvp& mvp) { impl->update(mvp); }
void MvpControl::Impl::update(Mvp& mvp) {
    if (pressed == false) {
        accumulated.x = accumulated.y = 0;
    }
    accumulated *= DIRECTION_SCALES;

    switch (mode) {
    case Mode::TranslateModel:
        mvp.update_translation(accumulated);
        break;
    case Mode::ScaleModel:
        mvp.update_scaling(accumulated);
        break;
    case Mode::RotateModel: {
        const auto [x, y, z] = accumulated * ROTATION_SCALE;
        mvp.update_rotation({y, x, z});
        break;
    }
    case Mode::TranslateEyePos:
        mvp.update_eyepos(accumulated);
        break;
    case Mode::TranslateViewCenter:
        mvp.update_center(accumulated);
        break;
    case Mode::TranslateViewUp:
        mvp.update_up(accumulated);
        break;
    }

    accumulated = Vector3{0, 0, 0};
}

void MvpControl::update_offset(Vector3 offset) { impl->update_offset(offset); }
void MvpControl::Impl::update_offset(Vector3 offset) { accumulated += offset; }

void MvpControl::set_pressed(bool pressed) { impl->set_pressed(pressed); }
void MvpControl::Impl::set_pressed(bool pressed) { this->pressed = pressed; }

void MvpControl::set_mode(Mode mode) { impl->set_mode(mode); }
void MvpControl::Impl::set_mode(Mode mode) { this->mode = mode; }
