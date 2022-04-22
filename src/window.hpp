#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <functional>
#include <memory>
#include <string>

constexpr int DEFAULT_WIDTH = 800;
constexpr int DEFAULT_HEIGHT = 600;

// Wrapper class for GLFW initialization / termination.
//
// Existence of an instance of this guarantees that the GLFW library has been initialized.
// Upon destruction, GLFW is terminated automatically.
class Glfw final {
  public:
    Glfw();
    ~Glfw();

    // Prevent copy
    Glfw(const Glfw&) = delete;
    Glfw& operator=(const Glfw&) = delete;

    // Allow move
    Glfw(Glfw&&) = default;
    Glfw& operator=(Glfw&&) = default;
};

// Callback types used in windows.
using KeyCallback = std::function<void(int, int, int, int)>;
using ScrollCallback = std::function<void(double, double)>;
using MouseButtonCallback = std::function<void(int, int, int)>;
using CursorPosCallback = std::function<void(double, double)>;
using FbSizeCallback = std::function<void(int, int)>;

// Wrapper class for GLFW windows.
class Window final {
  public:
    // Constructs a window and makes its associated context current.
    Window(const Glfw& glfw, std::string title, int width = DEFAULT_WIDTH,
           int height = DEFAULT_HEIGHT);
    ~Window();

    // Runs the main loop until the close flag of the window is set.
    void loop(std::function<void()> body);

    // Sets callbacks that are going to be called.
    // Must not be called while `loop` is running.
    void set_key_callback(KeyCallback callback);
    void set_scroll_callback(ScrollCallback callback);
    void set_mouse_button_callback(MouseButtonCallback callback);
    void set_cursor_pos_callback(CursorPosCallback callback);
    void set_fb_size_callback(FbSizeCallback callback);

  private:
    struct WindowImpl;
    std::unique_ptr<WindowImpl> impl;
};

#endif
