#include "window.hpp"

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <functional>
#include <optional>
#include <stdexcept>

Glfw::Glfw() {
    int res = glfwInit();
    if (res == GLFW_FALSE) {
        throw std::runtime_error("Failed to initialize GLFW");
    }
}

Glfw::~Glfw() { glfwTerminate(); }

namespace {
// Custom deleter to properly destroy raw glfw window
struct GlfwWindowDeleter {
    void operator()(GLFWwindow* ptr) { glfwDestroyWindow(ptr); }
};

} // namespace

struct Window::WindowImpl {
    WindowImpl(GLFWwindow* window) : window(window) {
        glfwSetWindowUserPointer(window, this);
        glfwSetKeyCallback(window, key_callback);
        glfwSetScrollCallback(window, scroll_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetCursorPosCallback(window, cursor_pos_callback);
        glfwSetFramebufferSizeCallback(window, fb_size_callback);
    }

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        auto impl = static_cast<Window::WindowImpl*>(glfwGetWindowUserPointer(window));
        if (auto callback = impl->_key_callback) {
            (*callback)(key, scancode, action, mods);
        }
    }

    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
        auto impl = static_cast<Window::WindowImpl*>(glfwGetWindowUserPointer(window));
        if (auto callback = impl->_scroll_callback) {
            (*callback)(xoffset, yoffset);
        }
    }

    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
        auto impl = static_cast<Window::WindowImpl*>(glfwGetWindowUserPointer(window));
        if (auto callback = impl->_mouse_button_callback) {
            (*callback)(button, action, mods);
        }
    }

    static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
        auto impl = static_cast<Window::WindowImpl*>(glfwGetWindowUserPointer(window));
        if (auto callback = impl->_cursor_pos_callback) {
            (*callback)(xpos, ypos);
        }
    }

    static void fb_size_callback(GLFWwindow* window, int width, int height) {
        auto impl = static_cast<Window::WindowImpl*>(glfwGetWindowUserPointer(window));
        if (auto callback = impl->_cursor_pos_callback) {
            (*callback)(width, height);
        }
    }

    std::unique_ptr<GLFWwindow, GlfwWindowDeleter> window;

    std::optional<KeyCallback> _key_callback;
    std::optional<ScrollCallback> _scroll_callback;
    std::optional<MouseButtonCallback> _mouse_button_callback;
    std::optional<CursorPosCallback> _cursor_pos_callback;
    std::optional<FbSizeCallback> _fb_size_callback;
};

Window::Window(const Glfw& glfw, std::string title, int width, int height) {
    // Set window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window and activate context
    GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(window);

    // Load OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLAD");
    }

    this->impl = std::make_unique<Window::WindowImpl>(window);
}

Window::~Window() {}

void Window::loop(std::function<void()> body) {
    while (glfwWindowShouldClose(impl->window.get()) == GLFW_FALSE) {
        body();
        glfwSwapBuffers(impl->window.get());
        glfwPollEvents();
    }
}

void Window::set_key_callback(KeyCallback callback) { impl->_key_callback = callback; }
void Window::set_scroll_callback(ScrollCallback callback) { impl->_scroll_callback = callback; }
void Window::set_mouse_button_callback(MouseButtonCallback callback) {
    impl->_mouse_button_callback = callback;
}
void Window::set_cursor_pos_callback(CursorPosCallback callback) {
    impl->_cursor_pos_callback = callback;
}
void Window::set_fb_size_callback(FbSizeCallback callback) { impl->_fb_size_callback = callback; }
