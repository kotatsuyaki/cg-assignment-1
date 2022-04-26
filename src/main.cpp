#include <cstdlib>
#include <exception>
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

Matrix4 view_matrix;
Matrix4 project_matrix;

// [TODO] given a translation vector then output a Matrix4 (Translation Matrix)
Matrix4 translate(Vector3 vec) {
    Matrix4 mat;

    /*
    mat = Matrix4(
            ...
    );
    */

    return mat;
}

// [TODO] given a scaling vector then output a Matrix4 (Scaling Matrix)
Matrix4 scaling(Vector3 vec) {
    Matrix4 mat;

    /*
    mat = Matrix4(
            ...
    );
    */

    return mat;
}

// [TODO] given a float value then ouput a rotation matrix alone axis-X (rotate
// alone axis-X)
Matrix4 rotate_x(float val) {
    Matrix4 mat;

    /*
    mat = Matrix4(
            ...
    );
    */

    return mat;
}

// [TODO] given a float value then ouput a rotation matrix alone axis-Y (rotate
// alone axis-Y)
Matrix4 rotate_y(float val) {
    Matrix4 mat;

    /*
    mat = Matrix4(
            ...
    );
    */

    return mat;
}

// [TODO] given a float value then ouput a rotation matrix alone axis-Z (rotate
// alone axis-Z)
Matrix4 rotate_z(float val) {
    Matrix4 mat;

    /*
    mat = Matrix4(
            ...
    );
    */

    return mat;
}

Matrix4 rotate(Vector3 vec) { return rotate_x(vec.x) * rotate_y(vec.y) * rotate_z(vec.z); }

void draw_plane() {
    float vertices[18]{
        1.0f, -0.9f, -1.0f, 1.0f,  -0.9f, 1.0f, -1.0f, -0.9f, -1.0f,
        1.0f, -0.9f, 1.0f,  -1.0f, -0.9f, 1.0f, -1.0f, -0.9f, -1.0f,
    };

    float colors[18]{
        0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.8f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.5f, 0.8f, 0.0f, 0.5f, 0.8f, 0.0f, 1.0f, 0.0f,
    };

    // [TODO] draw the plane with above vertices and color
}
