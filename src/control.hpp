#ifndef CONTORL_HPP_
#define CONTORL_HPP_

#include <memory>

#include "transform/mvp.hpp"

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
#endif
