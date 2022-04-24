#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <functional>
#include <memory>
#include <string>

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

// Key and action types to avoid leaking underlying impl
enum class Key { A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z };
enum class KeyAction { Down, Up };

// Callback types used in windows.
using KeyCallback = std::function<void()>;
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

    static const int DEFAULT_WIDTH;
    static const int DEFAULT_HEIGHT;

    // Prevent copy, allow move
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = default;
    Window& operator=(Window&&) = default;

    // Runs the main loop until the close flag of the window is set.
    void loop(std::function<void()> body) const;

    // Makes the context of this window the current context
    void make_current() const;

    // Sets callbacks.
    // Must not be called while `loop` is running.
    void on_key(Key key, KeyAction action, KeyCallback callback);
    void set_scroll_callback(ScrollCallback callback);
    void set_mouse_button_callback(MouseButtonCallback callback);
    void set_cursor_pos_callback(CursorPosCallback callback);
    void set_fb_size_callback(FbSizeCallback callback);

  private:
    struct WindowImpl;
    std::unique_ptr<WindowImpl> impl;
};

#endif
