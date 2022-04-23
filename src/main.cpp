#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "matrix.hpp"
#include "model.hpp"
#include "resources.hpp"
#include "shader.hpp"
#include "vector.hpp"
#include "window.hpp"

#include <glad/glad.h>

using std::cerr;
using std::cout;
using std::size_t;
using std::string;
using std::vector;
namespace fs = std::filesystem;

// Default window size
constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

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

GLint i_loc_mvp;

vector<string> filenames;

struct Camera {
    Vector3 position = Vector3{0.0f, 0.0f, 2.0f};
    Vector3 center = Vector3{0.0f, 0.0f, 0.0f};
    Vector3 up_vector = Vector3{0.0f, 1.0f, 0.0f};
};
Camera main_camera;

struct ProjectSettings {
    GLfloat near_clip = 0.001f;
    GLfloat far_clip = 100.0f;
    GLfloat fovy = 80;
    GLfloat aspect = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
    GLfloat left = -1;
    GLfloat right = 1;
    GLfloat top = 1;
    GLfloat bottom = -1;
};
ProjectSettings proj;

enum class ProjMode {
    Orthogonal,
    Perspective,
};
ProjMode cur_proj_mode = ProjMode::Orthogonal;
TransMode cur_trans_mode = TransMode::GeoTranslation;

Matrix4 view_matrix;
Matrix4 project_matrix;

GLvoid normalize(GLfloat v[3]) {
    GLfloat l;

    l = static_cast<GLfloat>(std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]));
    v[0] /= l;
    v[1] /= l;
    v[2] /= l;
}

GLvoid cross(GLfloat u[3], GLfloat v[3], GLfloat n[3]) {

    n[0] = u[1] * v[2] - u[2] * v[1];
    n[1] = u[2] * v[0] - u[0] * v[2];
    n[2] = u[0] * v[1] - u[1] * v[0];
}

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
Matrix4 rotate_x(GLfloat val) {
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
Matrix4 rotate_y(GLfloat val) {
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
Matrix4 rotate_z(GLfloat val) {
    Matrix4 mat;

    /*
    mat = Matrix4(
            ...
    );
    */

    return mat;
}

Matrix4 rotate(Vector3 vec) { return rotate_x(vec.x) * rotate_y(vec.y) * rotate_z(vec.z); }

// [TODO] compute viewing matrix accroding to the setting of main_camera
void set_viewing_matrix() {
    // view_matrix[...] = ...
}

// [TODO] compute orthogonal projection matrix
void set_orthogonal() {
    cur_proj_mode = ProjMode::Orthogonal;
    // project_matrix [...] = ...
}

// [TODO] compute persepective projection matrix
void set_perspective() {
    cur_proj_mode = ProjMode::Perspective;
    // project_matrix [...] = ...
}

// Vertex buffers
GLuint vao, vbo;

void draw_plane() {
    GLfloat vertices[18]{
        1.0f, -0.9f, -1.0f, 1.0f,  -0.9f, 1.0f, -1.0f, -0.9f, -1.0f,
        1.0f, -0.9f, 1.0f,  -1.0f, -0.9f, 1.0f, -1.0f, -0.9f, -1.0f,
    };

    GLfloat colors[18]{
        0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.8f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.5f, 0.8f, 0.0f, 0.5f, 0.8f, 0.0f, 1.0f, 0.0f,
    };

    // [TODO] draw the plane with above vertices and color
}

// Render function for display rendering
void render_scene(Model& model) {
    // clear canvas
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    Matrix4 t, r, s;
    // [TODO] update translation, rotation and scaling

    Matrix4 mvp;
    GLfloat mvp_arr[16];

    // [TODO] multiply all the matrix
    // [TODO] row-major ---> column-major

    mvp_arr[0] = 1;
    mvp_arr[4] = 0;
    mvp_arr[8] = 0;
    mvp_arr[12] = 0;
    mvp_arr[1] = 0;
    mvp_arr[5] = 1;
    mvp_arr[9] = 0;
    mvp_arr[13] = 0;
    mvp_arr[2] = 0;
    mvp_arr[6] = 0;
    mvp_arr[10] = 1;
    mvp_arr[14] = 0;
    mvp_arr[3] = 0;
    mvp_arr[7] = 0;
    mvp_arr[11] = 0;
    mvp_arr[15] = 1;

    // use uniform to send mvp to vertex shader
    glUniformMatrix4fv(i_loc_mvp, 1, GL_FALSE, mvp_arr);
    model.draw();
    draw_plane();
}

void init_parameter() {
    set_viewing_matrix();
    set_perspective(); // set default projection matrix as perspective matrix
}

ModelList load_models(const Window& window) {
    ModelList models{};
    vector<string> model_list{};
    for (auto const& entry : fs::recursive_directory_iterator("./")) {
        if (entry.path().extension().string() == ".obj") {
            auto path = entry.path().string();

            std::cout << "Loading model from path " << path << "\n";
            models.push_back(Model(window, path));
        }
    }
    return models;
}

int main(int argc, char** argv) {
    Glfw glfw{};
    Window window{glfw, "107021129 HW1"};

    window.set_key_callback([](int key, int scancode, int action, int mods) {});
    window.set_scroll_callback([](double xoffset, double yoffset) {});
    window.set_mouse_button_callback([](int button, int action, int mods) {});
    window.set_cursor_pos_callback([](double xpos, double ypos) {});
    window.set_fb_size_callback([](int width, int height) { glViewport(0, 0, width, height); });

    // Setup shader
    Shader shader{window, resources::SHADER_VS, resources::SHADER_FS};
    i_loc_mvp = shader.uniform_location("mvp");
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    // Load models
    ModelList models = load_models(window);

    // main loop
    window.loop([&models]() { render_scene(models.current()); });

    std::cout << sizeof(Model) << "\n";

    return 0;
}
