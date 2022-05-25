#include "control.hpp"

namespace {
// Used to slow down mouse operations.
const Vector3 DIRECTION_SCALES{0.01f, 0.01f, 0.01f};

// These constants makes the directions of mouse operations the same as the example binary.
// (and yes, they're non-intuitive)
const Vector3 TRANSLATE_SCALES{1.0f, 1.0f, -1.0f};
const Vector3 ROTATION_SCALES{-10.0f, 10.0f, -10.0f};
const Vector3 SCALING_SCALES{-1.0f, 1.0f, -1.0f};
const Vector3 EYEPOS_SCALES{-1.0f, -1.0f, 1.0f};
const Vector3 CENTER_SCALES{-1.0f, 1.0f, 1.0f};
const Vector3 UP_SCALES{-1.0f, -1.0f, 1.0f};
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
        accumulated.x() = accumulated.y() = 0;
    }
    accumulated *= DIRECTION_SCALES;

    switch (mode) {
    case Mode::TranslateModel: {
        accumulated *= TRANSLATE_SCALES;
        mvp.update_translation(accumulated);
    } break;
    case Mode::ScaleModel: {
        accumulated *= SCALING_SCALES;
        mvp.update_scaling(accumulated);
    } break;
    case Mode::RotateModel: {
        const auto [x, y, z] = accumulated * ROTATION_SCALES;
        // x and y are swapped intentionally
        mvp.update_rotation({y, x, z});
    } break;
    case Mode::TranslateEyePos: {
        accumulated *= EYEPOS_SCALES;
        mvp.update_eyepos(accumulated);
    } break;
    case Mode::TranslateViewCenter: {
        accumulated *= CENTER_SCALES;
        mvp.update_center(accumulated);
    } break;
    case Mode::TranslateViewUp: {
        accumulated *= UP_SCALES;
        mvp.update_up(accumulated);
    } break;
    }

    accumulated = Vector3{0, 0, 0};
}

void MvpControl::update_offset(Vector3 offset) { impl->update_offset(offset); }
void MvpControl::Impl::update_offset(Vector3 offset) { accumulated += offset; }

void MvpControl::set_pressed(bool pressed) { impl->set_pressed(pressed); }
void MvpControl::Impl::set_pressed(bool pressed) { this->pressed = pressed; }

void MvpControl::set_mode(Mode mode) { impl->set_mode(mode); }
void MvpControl::Impl::set_mode(Mode mode) { this->mode = mode; }
