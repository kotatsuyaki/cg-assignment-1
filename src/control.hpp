#ifndef CONTORL_HPP_
#define CONTORL_HPP_

#include <memory>

#include "shader.hpp"
#include "transform/mvp.hpp"

class LightOpts;

// Class making changes to instances of Mvp and other shader program parameters.
class Control {
  public:
    enum class Mode {
        TranslateModel,
        ScaleModel,
        RotateModel,
        TranslateEyePos,
        TranslateViewCenter,
        TranslateViewUp,
        Light,
        Shininess,
    };
    Control();
    ~Control();

    // Applies accumulated changes to mvp.
    // The accumulated changes are zeroed at the end.
    void update(Mvp& mvp, LightOpts& light);

    // Adds offset to the accumulated value.
    void update_offset(Vector3 offset);

    // Sets whether mouse button is pressed.
    void set_pressed(bool pressed);

    // Updates the mode of operation.
    void set_mode(Mode mode);

  private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

class LightMode {
  public:
    enum Value : int { Direction, Point, Spot };

    LightMode() = default;
    inline constexpr LightMode(Value value) : value(value) {}

    inline constexpr bool operator==(LightMode a) const { return value == a.value; }
    inline constexpr bool operator!=(LightMode a) const { return value != a.value; }

    inline int int_value() const { return value; }
    inline void next() { value = static_cast<Value>((int_value() + 1) % 3); }

  private:
    Value value;
};

struct LightOpts {
    Vector3 dir_light_pos{1.0f, 1.0f, 1.0f};
    Vector3 point_light_pos{0.0f, 2.0f, 1.0f};
    Vector3 spot_light_pos{0.0f, 0.0f, 2.0f};

    float shine = 64.0f;
    float cutoff = 30.0f;
    float diffuse = 1.0f;

    void set(LightMode mode, Shader& shader) const;
};

#endif
