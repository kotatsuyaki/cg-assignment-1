#ifndef CONTORL_HPP_
#define CONTORL_HPP_

#include <memory>

#include "transform/mvp.hpp"

// Class making changes to instances of Mvp.
class MvpControl {
  public:
    enum class Mode {
        TranslateModel,
        ScaleModel,
        RotateModel,
        TranslateEyePos,
        TranslateViewCenter,
        TranslateViewUp,
    };
    MvpControl();
    ~MvpControl();

    // Applies accumulated changes to mvp.
    // The accumulated changes are zeroed at the end.
    void update(Mvp& mvp);

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

#endif
