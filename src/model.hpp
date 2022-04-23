#ifndef MODEL_HPP_
#define MODEL_HPP_

#include <cstddef>
#include <memory>
#include <string_view>
#include <vector>

using std::size_t;

class Window;

// Wrapper class for OpenGL data buffers.
// Provides API to draw the buffers.
class Model final {
  public:
    Model(const Window& window, std::string_view path);
    ~Model();

    // Prevent copy, allow move
    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;
    Model(Model&&) = default;
    Model& operator=(Model&&) = default;

    // Draws the model using GL functions
    void draw() const;

  private:
    struct ModelImpl;
    std::unique_ptr<ModelImpl> impl;
};

// Container of models that provides ability to cycle through the models.
class ModelList final {
  public:
    ModelList();
    ~ModelList();

    // Pushes a new model into the list.
    void push_back(Model&& model);

    // Returns a reference to the current model.
    Model& current();

    // Switches current index to the next model.
    void next_model();

    // Prevent copy, allow move
    ModelList(const ModelList&) = delete;
    ModelList& operator=(const ModelList&) = delete;
    ModelList(ModelList&&) = default;
    ModelList& operator=(ModelList&&) = default;

  private:
    std::vector<Model> models;
    size_t index;
};

#endif
