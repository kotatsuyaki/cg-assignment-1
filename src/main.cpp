#include <cstdlib>
#include <exception>
#include <ios>
#include <iostream>
#include <memory>
#include <string>

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

    // Setup keyboard callbacks
    window.on_key(Key::Z, KeyAction::Down, [&]() { models.prev_model(); });
    window.on_key(Key::X, KeyAction::Down, [&]() { models.next_model(); });
    window.on_key(Key::W, KeyAction::Down, [&]() { scene.switch_render_mode(); });
    window.on_key(Key::I, KeyAction::Down, [&]() { mvp.debug_print(); });
    window.on_key(Key::O, KeyAction::Down,
                  [&]() { mvp.set_project_mode(Mvp::ProjectMode::Orthogonal); });
    window.on_key(Key::P, KeyAction::Down,
                  [&]() { mvp.set_project_mode(Mvp::ProjectMode::Perspective); });
    window.on_key(Key::V, KeyAction::Down, [&]() {
        const bool on = window.toggle_vsync();
        std::cerr << "Vsync: " << std::boolalpha << on << "\n";
    });

    // Setup other callbacks
    window.set_scroll_callback([](double xoffset, double yoffset) {});
    window.set_mouse_button_callback([](int button, int action, int mods) {});
    window.set_cursor_pos_callback([](double xpos, double ypos) {});

    window.set_fb_size_callback(
        [&](int width, int height) { mvp.set_viewport_size(width, height); });

    // main loop
    window.loop([&]() { scene.render(window, mvp); });

    return 0;
}

bool mouse_pressed = false;
int starting_press_x = -1;
int starting_press_y = -1;

enum class TransMode {
    GeoTranslation,
    GeoRotation,
    GeoScaling,
    ViewCenter,
    ViewEye,
    ViewUp,
};
