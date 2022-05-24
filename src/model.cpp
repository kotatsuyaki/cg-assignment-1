#include "model.hpp"

#include <exception>
#include <filesystem>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#define TINYOBJLOADER_IMPLEMENTATION
#include <glad/glad.h>
#include <tinyobjloader/tiny_obj_loader.h>

namespace fs = std::filesystem;

namespace {
void normalize(tinyobj::attrib_t* attrib, std::vector<GLfloat>& vertices,
               std::vector<GLfloat>& colors, tinyobj::shape_t* shape);
enum class LoadStatus {
    NotYet,
    Loaded,
    Failed,
};
} // namespace

struct Model::Impl {
    std::string path;

    mutable LoadStatus status;

    mutable GLuint vao;
    mutable GLuint vertices;
    mutable GLuint colors;
    mutable size_t vertex_count;

    Impl(std::string_view path);

    // Delete the OpenGL objects
    ~Impl();

    // Prevent copy and move
    Impl(const Impl&) = delete;
    Impl& operator=(const Impl&) = delete;
    Impl(Impl&&) = delete;
    Impl& operator=(Impl&&) = delete;

    void draw() const;
    void load() const;
    void unload() const;
};

Model::Impl::Impl(std::string_view path) : path(path), status(LoadStatus::NotYet) {}

Model::Impl::~Impl() { unload(); }

Model::Model(std::string_view path) : impl(std::make_shared<Impl>(path)) {}

void Model::draw() const { impl->draw(); }
void Model::Impl::draw() const {
    if (status == LoadStatus::NotYet) {
        try {
            load();
            status = LoadStatus::Loaded;
        } catch (const std::exception& e) {
            std::cerr << "Exception during model load:\n" << e.what() << "\n";
            status = LoadStatus::Failed;
        }
    }

    if (status == LoadStatus::Loaded) {
        glBindVertexArray(vao);
        // NOTE: We don't have boost::numeric_cast available.  This cast may overflow.
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertex_count));
    }
}

void Model::Impl::unload() const {
    if (status == LoadStatus::Loaded) {
        glDeleteBuffers(1, &vertices);
        glDeleteBuffers(1, &colors);
        glDeleteVertexArrays(1, &vao);
    }
}

void Model::Impl::load() const {
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    tinyobj::attrib_t attrib;
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> colors;

    std::string err, warn;

    const auto parent_path = fs::path(path).parent_path();
    bool res = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str(),
                                parent_path.string().c_str());

    if (res == false) {
        throw std::runtime_error(std::string("Failed to load object file:\n") + err + "\n" + warn);
    }

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

    std::cerr << "Loaded model from " << path << " successfully\n";
}

struct ModelList::Impl {
    std::vector<Model> models;
    size_t index;
    Impl(const std::vector<std::string>& model_paths);

    Impl(const Impl&) = delete;
    void operator=(const Impl&) = delete;
    Impl(Impl&&) = delete;
    void operator=(Impl&&) = delete;
};

ModelList::ModelList(const std::vector<std::string>& model_paths)
    : impl(std::make_shared<Impl>(model_paths)) {}
ModelList::Impl::Impl(const std::vector<std::string>& model_paths) : models(), index(0) {
    for (const auto& path : model_paths) {
        models.push_back(Model(path));
    }
}

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
