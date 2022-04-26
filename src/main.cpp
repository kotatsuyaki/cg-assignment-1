#include <cstdlib>
#include <exception>
#include <ios>
#include <iostream>
#include <memory>
#include <optional>
#include <string>

#include "control.hpp"
#include "matrix.hpp"
#include "model.hpp"
#include "prompt.hpp"
#include "resources.hpp"
#include "scene.hpp"
#include "shader.hpp"
#include "transform/mvp.hpp"
#include "vector.hpp"
#include "window.hpp"

using std::size_t;

int main(int argc, char** argv) {
    // Prompt for model path before GLFW window creation
    const auto model_paths = prompt_model_paths();

    Glfw glfw{};
    Window window{glfw, "107021129 HW1"};

    // Setup shader
    Shader shader{window, resources::SHADER_VS, resources::SHADER_FS};

    // Load models
    ModelList models{model_paths};

    // Setup scene
    std::unique_ptr<Drawable> models_cloned = std::make_unique<ModelList>(models);
    Scene scene{std::move(shader), Vector3{0.2f, 0.2f, 0.2f}, std::move(models_cloned)};

    // Setup transformation object
    Mvp mvp{Window::DEFAULT_WIDTH, Window::DEFAULT_HEIGHT};
    MvpControl control{};

    // Setup keyboard callbacks
    window.on_keydown(Key::Z, [&]() { models.prev_model(); });
    window.on_keydown(Key::X, [&]() { models.next_model(); });
    window.on_keydown(Key::W, [&]() { scene.switch_render_mode(); });
    window.on_keydown(Key::I, [&]() { mvp.debug_print(); });
    window.on_keydown(Key::O, [&]() { mvp.set_project_mode(Mvp::ProjectMode::Orthogonal); });
    window.on_keydown(Key::P, [&]() { mvp.set_project_mode(Mvp::ProjectMode::Perspective); });

    window.on_keydown(Key::T, [&]() { control.set_mode(MvpControl::Mode::TranslateModel); });
    window.on_keydown(Key::R, [&]() { control.set_mode(MvpControl::Mode::RotateModel); });
    window.on_keydown(Key::S, [&]() { control.set_mode(MvpControl::Mode::ScaleModel); });
    window.on_keydown(Key::E, [&]() { control.set_mode(MvpControl::Mode::TranslateEyePos); });
    window.on_keydown(Key::C, [&]() { control.set_mode(MvpControl::Mode::TranslateViewCenter); });
    window.on_keydown(Key::U, [&]() { control.set_mode(MvpControl::Mode::TranslateViewUp); });

    window.on_keydown(Key::V, [&]() {
        const bool on = window.toggle_vsync();
        std::cerr << "Vsync: " << std::boolalpha << on << "\n";
    });

    // Setup mouse callbacks
    window.set_scroll_callback([&](float xoffset, float yoffset) {
        control.update_offset({0, 0, -yoffset});
    });

    window.set_mouse_button_callback([&](KeyAction action) {
        switch (action) {
        case KeyAction::Down:
            control.set_pressed(true);
            break;
        case KeyAction::Up:
            control.set_pressed(false);
            break;
        }
    });

    std::optional<std::pair<float, float>> last_cursor_pos;
    window.set_cursor_pos_callback([&](float xpos, float ypos) {
        auto [last_xpos, last_ypos] = last_cursor_pos.value_or(std::make_pair(xpos, ypos));
        // Offset along y-axis is negated, since glfw reports larger number when cursur is lower
        control.update_offset({xpos - last_xpos, last_ypos - ypos, 0});
        last_cursor_pos = {xpos, ypos};
    });

    window.set_fb_size_callback(
        [&](int width, int height) { mvp.set_viewport_size(width, height); });

    // Run the main loop
    window.loop([&]() {
        control.update(mvp);
        scene.render(window, mvp);
    });

    return 0;
}
