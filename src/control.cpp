#include "control.hpp"
#include "shader.hpp"

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
const Vector3 LIGHT_SCALES{1.0f, 1.0f, -1.0f};
const Vector3 SHINE_SCALES{1.0f, 1.0f, -500.0f};
} // namespace

class Control::Impl {
  public:
    Impl();
    void update(Mvp& mvp, LightOpts& light);
    void update_offset(Vector3 offset);
    void set_pressed(bool pressed);
    void set_mode(Mode mode);

  private:
    Mode mode;
    Vector3 accumulated;
    bool pressed;
};

Control::Control() : impl(std::make_unique<Impl>()) {}
Control::Impl::Impl() : mode(Mode::TranslateModel), accumulated({0, 0, 0}), pressed(false) {}

Control::~Control() = default;

void Control::update(Mvp& mvp, LightOpts& light) { impl->update(mvp, light); }
void Control::Impl::update(Mvp& mvp, LightOpts& light) {
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
    case Mode::Light: {
        accumulated *= LIGHT_SCALES;
        const Vector3 displace{accumulated.x(), accumulated.y(), 0.0f};

        light.dir_light_pos += displace;
        light.point_light_pos += displace;
        light.spot_light_pos += displace;

        light.diffuse += accumulated.z();
        light.cutoff += accumulated.z();
    } break;
    case Mode::Shininess: {
        accumulated *= SHINE_SCALES;
        light.shine += accumulated.z();
    } break;
    }

    accumulated = Vector3{0, 0, 0};
}

void Control::update_offset(Vector3 offset) { impl->update_offset(offset); }
void Control::Impl::update_offset(Vector3 offset) { accumulated += offset; }

void Control::set_pressed(bool pressed) { impl->set_pressed(pressed); }
void Control::Impl::set_pressed(bool pressed) { this->pressed = pressed; }

void Control::set_mode(Mode mode) { impl->set_mode(mode); }
void Control::Impl::set_mode(Mode mode) { this->mode = mode; }

void LightOpts::set(LightMode mode, Shader& shader) const {
    shader.set_uniform("diffuse", diffuse);
    shader.set_uniform("cutoff", cutoff);
    shader.set_uniform("shine", shine);

    if (mode == LightMode::Direction) {
        shader.set_uniform("light_pos", dir_light_pos);
    } else if (mode == LightMode::Point) {
        shader.set_uniform("light_pos", point_light_pos);
    } else if (mode == LightMode::Spot) {
        shader.set_uniform("light_pos", spot_light_pos);
    }
}
