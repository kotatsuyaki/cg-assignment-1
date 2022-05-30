#include "model.hpp"
#include "vector.hpp"

#include <exception>
#include <filesystem>
#include <iostream>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#define TINYOBJLOADER_IMPLEMENTATION
#include <glad/glad.h>
#include <stb/stb_image.h>
#include <tinyobjloader/tiny_obj_loader.h>

using std::optional;
using std::vector;

namespace fs = std::filesystem;

namespace {
void normalize(tinyobj::attrib_t* attrib, std::vector<GLfloat>& vertices,
               std::vector<GLfloat>& colors, std::vector<GLfloat>& normals,
               vector<GLfloat>& texture_coords, vector<int>& material_ids, tinyobj::shape_t* shape);
GLuint load_texture_image(const fs::path& image_path);

enum class LoadStatus {
    NotYet,
    Loaded,
    Failed,
};

struct Material {
    // Ambient color
    Vector3 ka{1, 1, 1};
    // Diffuse color
    Vector3 kd{1, 1, 1};
    // Specular color
    Vector3 ks{1, 1, 1};

    optional<GLuint> texture;
};

struct SubModel {
    Material material;
    GLuint vao = 0;
    GLuint vertices = 0;
    GLuint colors = 0;
    GLuint normals = 0;
    GLuint texcoords = 0;
    size_t vertex_count = 0;
};

} // namespace

struct Model::Impl {
    std::string path;

    mutable LoadStatus status;

    mutable vector<SubModel> submodels;
    mutable vector<Material> materials;

    Impl(std::string_view path);

    // Delete the OpenGL objects
    ~Impl();

    // Prevent copy and move
    Impl(const Impl&) = delete;
    Impl& operator=(const Impl&) = delete;
    Impl(Impl&&) = delete;
    Impl& operator=(Impl&&) = delete;

    void draw(Shader& shader) const;
    void load() const;
    void unload() const;

    void populate_submodels(vector<GLfloat>& vertices, vector<GLfloat>& colors,
                            vector<GLfloat>& normals, vector<GLfloat>& texture_coords,
                            vector<int>& material_id) const;
};

Model::Impl::Impl(std::string_view path) : path(path), status(LoadStatus::NotYet) {}

Model::Impl::~Impl() { unload(); }

Model::Model(std::string_view path) : impl(std::make_shared<Impl>(path)) {}

void Model::draw(Shader& shader) const { impl->draw(shader); }
void Model::Impl::draw(Shader& shader) const {
    if (status == LoadStatus::NotYet) {
        try {
            load();
            status = LoadStatus::Loaded;
        } catch (const std::exception& e) {
            std::cerr << "Exception during model load from " << path << ":\n" << e.what() << "\n";
            status = LoadStatus::Failed;
        }
    }

    if (status == LoadStatus::Loaded) {
        for (const auto& submodel : submodels) {
            glBindVertexArray(submodel.vao);

            shader.set_uniform("ka", submodel.material.ka);
            shader.set_uniform("kd", submodel.material.kd);
            shader.set_uniform("ks", submodel.material.ks);

            if (auto texture = submodel.material.texture) {
                shader.set_uniform("tex_loaded", 1);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, *texture);
            } else {
                shader.set_uniform("tex_loaded", 0);
            }

            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(submodel.vertex_count));
        }
    }
}

void Model::Impl::unload() const {
    if (status == LoadStatus::Loaded) {
        for (const auto& submodel : submodels) {
            glDeleteBuffers(1, &submodel.vertices);
            glDeleteBuffers(1, &submodel.colors);
            glDeleteBuffers(1, &submodel.normals);
            glDeleteBuffers(1, &submodel.texcoords);
            glDeleteVertexArrays(1, &submodel.vao);
        }
    }
}

void Model::Impl::load() const {
    vector<tinyobj::shape_t> shapes;
    vector<tinyobj::material_t> raw_materials;
    tinyobj::attrib_t attrib;
    vector<GLfloat> vertices;
    vector<GLfloat> colors;
    vector<GLfloat> normals;
    vector<GLfloat> texture_coords;
    vector<int> material_ids;

    std::string err, warn;

    const auto parent_path = fs::path(path).parent_path();
    bool res = tinyobj::LoadObj(&attrib, &shapes, &raw_materials, &warn, &err, path.c_str(),
                                parent_path.string().c_str());

    if (res == false) {
        throw std::runtime_error(std::string("Failed to load object file:\n") + err + "\n" + warn);
    }

    for (const auto& raw_mat : raw_materials) {
        Material mat;
        mat.ka = Vector3{raw_mat.ambient};
        mat.kd = Vector3{raw_mat.diffuse};
        mat.ks = Vector3{raw_mat.specular};

        const auto diffuse_texture_path = parent_path / raw_mat.diffuse_texname;
        try {
            mat.texture = load_texture_image(diffuse_texture_path);
        } catch (std::exception& e) {
            std::cerr << "Failed to load texture:\n" << e.what() << "\n";
            mat.texture = std::nullopt;
        }

        materials.push_back(mat);
    }

    normalize(&attrib, vertices, colors, normals, texture_coords, material_ids, &shapes[0]);
    populate_submodels(vertices, colors, normals, texture_coords, material_ids);

    std::cerr << "Loaded model from " << path << " successfully\n";
}

void Model::Impl::populate_submodels(vector<GLfloat>& vertices, vector<GLfloat>& colors,
                                     vector<GLfloat>& normals, vector<GLfloat>& texture_coords,
                                     vector<int>& material_ids) const {
    for (int m = 0; m < materials.size(); m++) {
        vector<GLfloat> m_vertices, m_colors, m_normals, m_texture_coords;
        for (int v = 0; v < material_ids.size(); v++) {
            if (material_ids[v] == m) {
                m_vertices.push_back(vertices[v * 3 + 0]);
                m_vertices.push_back(vertices[v * 3 + 1]);
                m_vertices.push_back(vertices[v * 3 + 2]);

                m_colors.push_back(colors[v * 3 + 0]);
                m_colors.push_back(colors[v * 3 + 1]);
                m_colors.push_back(colors[v * 3 + 2]);

                m_normals.push_back(normals[v * 3 + 0]);
                m_normals.push_back(normals[v * 3 + 1]);
                m_normals.push_back(normals[v * 3 + 2]);

                m_texture_coords.push_back(texture_coords[v * 2 + 0]);
                m_texture_coords.push_back(texture_coords[v * 2 + 1]);
            }
        }

        if (!m_vertices.empty()) {
            SubModel submodel;

            submodel.vertex_count = m_vertices.size() / 3;

            glGenVertexArrays(1, &submodel.vao);
            glBindVertexArray(submodel.vao);

            const auto gen_and_bind = [](GLuint index, GLint size, GLuint& buffer, auto& vec) {
                glGenBuffers(1, &buffer);
                glBindBuffer(GL_ARRAY_BUFFER, buffer);
                glBufferData(GL_ARRAY_BUFFER, vec.size() * sizeof(GLfloat), &vec.at(0),
                             GL_STATIC_DRAW);

                glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, 0, 0);
                glEnableVertexAttribArray(index);
            };

            gen_and_bind(0, 3, submodel.vertices, m_vertices);
            gen_and_bind(1, 3, submodel.colors, m_colors);
            gen_and_bind(2, 3, submodel.normals, m_normals);
            gen_and_bind(3, 2, submodel.texcoords, m_texture_coords);

            submodel.material = materials[m];
            submodels.push_back(submodel);
        }
    }
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

void ModelList::draw(Shader& shader) const { current().draw(shader); }

namespace {
void normalize(tinyobj::attrib_t* attrib, std::vector<GLfloat>& vertices,
               std::vector<GLfloat>& colors, std::vector<GLfloat>& normals,
               vector<GLfloat>& texture_coords, vector<int>& material_ids,
               tinyobj::shape_t* shape) {
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

            colors.push_back(attrib->colors[3 * idx.vertex_index + 0]);
            colors.push_back(attrib->colors[3 * idx.vertex_index + 1]);
            colors.push_back(attrib->colors[3 * idx.vertex_index + 2]);

            normals.push_back(attrib->normals[3 * idx.normal_index + 0]);
            normals.push_back(attrib->normals[3 * idx.normal_index + 1]);
            normals.push_back(attrib->normals[3 * idx.normal_index + 2]);

            texture_coords.push_back(attrib->texcoords[2 * idx.texcoord_index + 0]);
            texture_coords.push_back(attrib->texcoords[2 * idx.texcoord_index + 1]);

            material_ids.push_back(shape->mesh.material_ids[f]);
        }
        index_offset += fv;
    }
}

GLuint load_texture_image(const fs::path& image_path) {
    constexpr int require_channel = 4;
    int channel, width, height;

    stbi_set_flip_vertically_on_load(true);

    stbi_uc* data = stbi_load(image_path.c_str(), &width, &height, &channel, require_channel);
    if (data != NULL) {
        GLuint tex = 0;

        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
        return tex;
    } else {
        throw std::runtime_error(std::string("Failed to load image from path") +
                                 image_path.string());
    }
}
} // namespace
