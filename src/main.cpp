#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "matrix.hpp"
#include "resources.hpp"
#include "shader.hpp"
#include "vector.hpp"
#include "window.hpp"

#include <glad/glad.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>

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

struct Model {
    Vector3 position = Vector3(0, 0, 0);
    Vector3 scale = Vector3(1, 1, 1);
    Vector3 rotation = Vector3(0, 0, 0); // Euler form
};
vector<Model> models;

struct Camera {
    Vector3 position;
    Vector3 center;
    Vector3 up_vector;
};
Camera main_camera;

struct ProjectSettings {
    GLfloat nearClip, farClip;
    GLfloat fovy;
    GLfloat aspect;
    GLfloat left, right, top, bottom;
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

struct Shape {
    GLuint vao;
    GLuint vbo;
    GLuint vboTex;
    GLuint ebo;
    GLuint p_color;
    int vertex_count;
    GLuint p_normal;
    int materialId;
    int indexCount;
    GLuint m_texture;
};
Shape quad;
Shape m_shpae;
vector<Shape> m_shape_list;
int cur_idx = 0; // represent which model should be rendered now

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
    GLfloat vertices[18]{1.0, -0.9, -1.0, 1.0,  -0.9, 1.0, -1.0, -0.9, -1.0,
                         1.0, -0.9, 1.0,  -1.0, -0.9, 1.0, -1.0, -0.9, -1.0};

    GLfloat colors[18]{0.0, 1.0, 0.0, 0.0, 0.5, 0.8, 0.0, 1.0, 0.0,
                       0.0, 0.5, 0.8, 0.0, 0.5, 0.8, 0.0, 1.0, 0.0};

    // [TODO] draw the plane with above vertices and color
}

// Render function for display rendering
void render_scene() {
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
    glBindVertexArray(m_shape_list[cur_idx].vao);
    glDrawArrays(GL_TRIANGLES, 0, m_shape_list[cur_idx].vertex_count);
    draw_plane();
}

void normalization(tinyobj::attrib_t* attrib, vector<GLfloat>& vertices, vector<GLfloat>& colors,
                   tinyobj::shape_t* shape) {
    vector<float> xs, ys, zs;
    float min_x = 10000, max_x = -10000, min_y = 10000, max_y = -10000, min_z = 10000,
          max_z = -10000;

    // find out min and max value of X, Y and Z axis
    for (int i = 0; i < attrib->vertices.size(); i++) {
        // maxs = max(maxs, attrib->vertices.at(i));
        if (i % 3 == 0) {

            xs.push_back(attrib->vertices.at(i));

            if (attrib->vertices.at(i) < min_x) {
                min_x = attrib->vertices.at(i);
            }

            if (attrib->vertices.at(i) > max_x) {
                max_x = attrib->vertices.at(i);
            }
        } else if (i % 3 == 1) {
            ys.push_back(attrib->vertices.at(i));

            if (attrib->vertices.at(i) < min_y) {
                min_y = attrib->vertices.at(i);
            }

            if (attrib->vertices.at(i) > max_y) {
                max_y = attrib->vertices.at(i);
            }
        } else if (i % 3 == 2) {
            zs.push_back(attrib->vertices.at(i));

            if (attrib->vertices.at(i) < min_z) {
                min_z = attrib->vertices.at(i);
            }

            if (attrib->vertices.at(i) > max_z) {
                max_z = attrib->vertices.at(i);
            }
        }
    }

    float offset_x = (max_x + min_x) / 2;
    float offset_y = (max_y + min_y) / 2;
    float offset_z = (max_z + min_z) / 2;

    for (int i = 0; i < attrib->vertices.size(); i++) {
        if (offset_x != 0 && i % 3 == 0) {
            attrib->vertices.at(i) = attrib->vertices.at(i) - offset_x;
        } else if (offset_y != 0 && i % 3 == 1) {
            attrib->vertices.at(i) = attrib->vertices.at(i) - offset_y;
        } else if (offset_z != 0 && i % 3 == 2) {
            attrib->vertices.at(i) = attrib->vertices.at(i) - offset_z;
        }
    }

    float greatest_axis = max_x - min_x;
    float dist_y_axis = max_y - min_y;
    float dist_z_axis = max_z - min_z;

    if (dist_y_axis > greatest_axis) {
        greatest_axis = dist_y_axis;
    }

    if (dist_z_axis > greatest_axis) {
        greatest_axis = dist_z_axis;
    }

    float scale = greatest_axis / 2;

    for (int i = 0; i < attrib->vertices.size(); i++) {
        // std::cout << i << " = " << (double)(attrib.vertices.at(i) /
        // greatestAxis)
        // << "\n";
        attrib->vertices.at(i) = attrib->vertices.at(i) / scale;
    }
    size_t index_offset = 0;
    vertices.reserve(shape->mesh.num_face_vertices.size() * 3);
    colors.reserve(shape->mesh.num_face_vertices.size() * 3);
    for (size_t f = 0; f < shape->mesh.num_face_vertices.size(); f++) {
        int fv = shape->mesh.num_face_vertices[f];

        // Loop over vertices in the face.
        for (size_t v = 0; v < fv; v++) {
            // access to vertex
            tinyobj::index_t idx = shape->mesh.indices[index_offset + v];
            vertices.push_back(attrib->vertices[3 * idx.vertex_index + 0]);
            vertices.push_back(attrib->vertices[3 * idx.vertex_index + 1]);
            vertices.push_back(attrib->vertices[3 * idx.vertex_index + 2]);
            // Optional: vertex colors
            colors.push_back(attrib->colors[3 * idx.vertex_index + 0]);
            colors.push_back(attrib->colors[3 * idx.vertex_index + 1]);
            colors.push_back(attrib->colors[3 * idx.vertex_index + 2]);
        }
        index_offset += fv;
    }
}

void load_models(string model_path) {
    vector<tinyobj::shape_t> shapes;
    vector<tinyobj::material_t> materials;
    tinyobj::attrib_t attrib;
    vector<GLfloat> vertices;
    vector<GLfloat> colors;

    string err;
    string warn;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, model_path.c_str());

    if (!warn.empty()) {
        cout << warn << "\n";
    }

    if (!err.empty()) {
        cerr << err << "\n";
    }

    if (!ret) {
        std::exit(1);
    }

    std::cout << "Load model success (shapes size = " << shapes.size()
              << ", material size = " << materials.size() << ")\n";

    normalization(&attrib, vertices, colors, &shapes[0]);

    Shape tmp_shape;
    glGenVertexArrays(1, &tmp_shape.vao);
    glBindVertexArray(tmp_shape.vao);

    glGenBuffers(1, &tmp_shape.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, tmp_shape.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices.at(0),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    tmp_shape.vertex_count = vertices.size() / 3;

    glGenBuffers(1, &tmp_shape.p_color);
    glBindBuffer(GL_ARRAY_BUFFER, tmp_shape.p_color);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(GLfloat), &colors.at(0), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    m_shape_list.push_back(tmp_shape);
    Model tmp_model;
    models.push_back(tmp_model);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    shapes.clear();
    materials.clear();
}

void init_parameter() {
    proj.left = -1;
    proj.right = 1;
    proj.top = 1;
    proj.bottom = -1;
    proj.nearClip = 0.001;
    proj.farClip = 100.0;
    proj.fovy = 80;
    proj.aspect = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;

    main_camera.position = Vector3(0.0f, 0.0f, 2.0f);
    main_camera.center = Vector3(0.0f, 0.0f, 0.0f);
    main_camera.up_vector = Vector3(0.0f, 1.0f, 0.0f);

    set_viewing_matrix();
    set_perspective(); // set default projection matrix as perspective matrix
}

void gl_print_context_info(bool print_extension) {
    cout << "GL_VENDOR = " << glGetString(GL_VENDOR) << "\n";
    cout << "GL_RENDERER = " << glGetString(GL_RENDERER) << "\n";
    cout << "GL_VERSION = " << glGetString(GL_VERSION) << "\n";
    cout << "GL_SHADING_LANGUAGE_VERSION = " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
    if (print_extension) {
        GLint num_ext;
        glGetIntegerv(GL_NUM_EXTENSIONS, &num_ext);
        cout << "GL_EXTENSIONS ="
             << "\n";
        for (GLint i = 0; i < num_ext; i++) {
            cout << "\t" << glGetStringi(GL_EXTENSIONS, i) << "\n";
        }
    }
}

int main(int argc, char** argv) {
    Glfw glfw{};
    Window window{glfw, "107021129 HW1"};

    window.set_key_callback([](int key, int scancode, int action, int mods) {});
    window.set_scroll_callback([](double xoffset, double yoffset) {});
    window.set_mouse_button_callback([](int button, int action, int mods) {});
    window.set_cursor_pos_callback([](double xpos, double ypos) {});
    window.set_fb_size_callback([](int width, int height) { glViewport(0, 0, width, height); });

    window.make_current();

    Shader shader{window, resources::SHADER_VS, resources::SHADER_FS};
    i_loc_mvp = shader.uniform_location("mvp");
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2, 0.2, 0.2, 1.0);

    vector<string> model_list{};
    for (auto const& entry : fs::recursive_directory_iterator("./")) {
        if (entry.path().extension().string() == ".obj") {
            cout << entry.path().string() << "\n";
            model_list.push_back(entry.path().string());
        }
    }
    load_models(model_list[cur_idx]);

    // main loop
    window.loop([]() { render_scene(); });

    return 0;
}
