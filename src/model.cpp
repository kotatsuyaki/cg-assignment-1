#include "model.hpp"

#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#define TINYOBJLOADER_IMPLEMENTATION
#include <glad/glad.h>
#include <tinyobjloader/tiny_obj_loader.h>

namespace {
void normalize(tinyobj::attrib_t* attrib, std::vector<GLfloat>& vertices,
               std::vector<GLfloat>& colors, tinyobj::shape_t* shape);
} // namespace

struct Model::ModelImpl {
    GLuint vao;
    GLuint vertices;
    GLuint colors;
    size_t vertex_count;

    ModelImpl(const Window& window, std::string_view path);

    // Delete the OpenGL objects
    ~ModelImpl();

    // Prevent copy and move
    ModelImpl(const ModelImpl&) = delete;
    ModelImpl& operator=(const ModelImpl&) = delete;
    ModelImpl(ModelImpl&&) = delete;
    ModelImpl& operator=(ModelImpl&&) = delete;
};

Model::ModelImpl::ModelImpl(const Window& window, std::string_view path) {
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    tinyobj::attrib_t attrib;
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> colors;

    std::string err, warn;

    bool res = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.data());

    if (!warn.empty()) {
        std::cerr << "tinyobjloader warning:\n" << warn << "\n";
    }

    if (!err.empty()) {
        std::cerr << "tinyobjloader error:\n" << err << "\n";
    }

    if (res == false) {
        throw std::runtime_error("Failed to load object file");
    }

    std::cout << "Load model success (shapes size = " << shapes.size()
              << ", material size = " << materials.size() << ")\n";

    normalize(&attrib, vertices, colors, &shapes[0]);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &this->vertices);
    glBindBuffer(GL_ARRAY_BUFFER, (this->vertices));
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices.at(0),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    vertex_count = vertices.size() / 3;

    glGenBuffers(1, &this->colors);
    glBindBuffer(GL_ARRAY_BUFFER, this->colors);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(GLfloat), &colors.at(0), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}

Model::ModelImpl::~ModelImpl() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vertices);
    glDeleteBuffers(1, &colors);
}

Model::Model(const Window& window, std::string_view path)
    : impl(std::make_shared<ModelImpl>(window, path)) {}

void Model::draw() const {
    glBindVertexArray(impl->vao);
    // NOTE: We don't have boost::numeric_cast available.  This cast may overflow.
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(impl->vertex_count));
}

struct ModelList::ModelListImpl {
    std::vector<Model> models;
    size_t index;
    ModelListImpl() : models(), index() {}

    ModelListImpl(const ModelListImpl&) = delete;
    void operator=(const ModelListImpl&) = delete;
    ModelListImpl(ModelListImpl&&) = delete;
    void operator=(ModelListImpl&&) = delete;
};

ModelList::ModelList() : impl(std::make_shared<ModelListImpl>()) {}

void ModelList::push_back(Model&& model) { impl->models.emplace_back(std::move(model)); }

const Model& ModelList::current() const {
    if (impl->models.size() == 0) {
        throw std::runtime_error("No model loaded");
    }
    return impl->models.at(impl->index);
}

void ModelList::next_model() {
    impl->index += 1;
    impl->index %= impl->models.size();
}

void ModelList::prev_model() {
    if (impl->index == 0) {
        impl->index = impl->models.size() - 1;
    } else {
        impl->index -= 1;
    }
}

void ModelList::draw() const { current().draw(); }

namespace {
void normalize(tinyobj::attrib_t* attrib, std::vector<GLfloat>& vertices,
               std::vector<GLfloat>& colors, tinyobj::shape_t* shape) {
    std::vector<float> xs, ys, zs;
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
} // namespace
