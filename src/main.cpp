#include <exception>
#include <iostream>
#include <memory>
#include <optional>

#include "control.hpp"
#include "matrix.hpp"
#include "model.hpp"
#include "prompt.hpp"
#include "resources.hpp"
#include "scene.hpp"
#include "shader.hpp"
#include "transform/mvp.hpp"
#include "window.hpp"

using std::size_t;

void init();

int main() {
    try {
        init();
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << "\n";
        return 1;
    }
    return 0;
}

void init() {
    // Prompt for model path before GLFW window creation
    const auto model_paths = prompt_model_paths();

    // Initialize glfw and window
    Glfw glfw{};
    Window window{glfw, "107021129 HW1"};

    // Setup shader
    Shader shader{window, resources::SHADER_VS, resources::SHADER_FS};

    // Load models
    ModelList models{model_paths};

    // Setup scene
    Scene scene{std::move(shader), std::make_unique<ModelList>(models)};

    // Setup transformation and control objects
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
    window.on_scroll([&](float xoffset, float yoffset) {
        control.update_offset({0, 0, -yoffset});
    });

    window.on_mouse([&](KeyAction action) { control.set_pressed(action == KeyAction::Down); });

    std::optional<std::pair<float, float>> last_cursor_pos;
    window.on_cursor_move([&](float xpos, float ypos) {
        const auto [last_xpos, last_ypos] = last_cursor_pos.value_or(std::make_pair(xpos, ypos));
        // Offset along y-axis is negated, since glfw reports larger number when cursur is lower
        control.update_offset({xpos - last_xpos, last_ypos - ypos, 0});
        last_cursor_pos = {xpos, ypos};
    });

    window.on_size_change([&](int width, int height) { mvp.set_viewport_size(width / 2, height); });

    // Run the main loop
    window.loop(
        [&]() {
            control.update(mvp);
            scene.render(window, mvp);
        },
        [&]() {
            control.update(mvp);
            scene.render(window, mvp);
        });
}
