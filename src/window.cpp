#include "window.hpp"

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <functional>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <unordered_map>

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

// Hasher for std::pair<_, _>
struct PairHash {
    template <class T, class U> size_t operator()(const std::pair<T, U>& p) const {
        size_t thash = std::hash<T>{}(p.first);
        size_t uhash = std::hash<U>{}(p.second);

        size_t magic;
        // Use 64-bit magic when size_t is 64-bit, but fallback to 32-bit magic
        if constexpr (sizeof(size_t) == 8) {
            magic = 0x9e3779b97f4a7c15;
        } else {
            magic = 0x9e3779b9;
        }
        return thash ^ uhash + magic + (thash << 6) + (thash >> 2);
    }
};

// Convert raw GLFW_KEY_* to safe types
std::optional<Key> int_to_key(int raw);
std::optional<KeyAction> int_to_action(int raw);
} // namespace

struct Window::Impl {
    Impl(GLFWwindow* window) : window(window), key_callbacks(), vsync(true) {
        glfwSetWindowUserPointer(window, this);
        glfwSetKeyCallback(window, key_callback);
        glfwSetScrollCallback(window, scroll_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetCursorPosCallback(window, cursor_pos_callback);
        glfwSetFramebufferSizeCallback(window, fb_size_callback);
    }

    static void key_callback(GLFWwindow* window, int raw_key, int scancode, int raw_action,
                             int mods) {
        auto impl = static_cast<Window::Impl*>(glfwGetWindowUserPointer(window));
        auto key = int_to_key(raw_key);
        auto action = int_to_action(raw_action);

        if (key.has_value() == false || action.has_value() == false) {
            return;
        }

        auto it = impl->key_callbacks.find({*key, *action});
        if (it != impl->key_callbacks.end()) {
            auto callback = it->second;
            callback();
        }
    }

    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
        auto impl = static_cast<Window::Impl*>(glfwGetWindowUserPointer(window));
        if (auto callback = impl->_scroll_callback) {
            (*callback)(xoffset, yoffset);
        }
    }

    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
        auto impl = static_cast<Window::Impl*>(glfwGetWindowUserPointer(window));
        if (auto callback = impl->_mouse_button_callback) {
            (*callback)(button, action, mods);
        }
    }

    static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
        auto impl = static_cast<Window::Impl*>(glfwGetWindowUserPointer(window));
        if (auto callback = impl->_cursor_pos_callback) {
            (*callback)(xpos, ypos);
        }
    }

    static void fb_size_callback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        auto impl = static_cast<Window::Impl*>(glfwGetWindowUserPointer(window));
        if (auto callback = impl->_fb_size_callback) {
            (*callback)(width, height);
        }
    }

    std::unique_ptr<GLFWwindow, GlfwWindowDeleter> window;

    std::optional<KeyCallback> _key_callback;
    std::optional<ScrollCallback> _scroll_callback;
    std::optional<MouseButtonCallback> _mouse_button_callback;
    std::optional<CursorPosCallback> _cursor_pos_callback;
    std::optional<FbSizeCallback> _fb_size_callback;

    std::unordered_map<std::pair<Key, KeyAction>, KeyCallback, PairHash> key_callbacks;

    bool vsync;
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
    glfwSwapInterval(1);

    // Load OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLAD");
    }

    this->impl = std::make_unique<Window::Impl>(window);
}

Window::~Window() {}

const int Window::DEFAULT_WIDTH = 800;
const int Window::DEFAULT_HEIGHT = 600;

void Window::loop(std::function<void()> body) const {
    auto window = impl->window.get();
    glfwMakeContextCurrent(window);

    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        body();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Window::make_current() const {
    auto window = impl->window.get();
    glfwMakeContextCurrent(window);
}

void Window::set_scroll_callback(ScrollCallback callback) { impl->_scroll_callback = callback; }
void Window::set_mouse_button_callback(MouseButtonCallback callback) {
    impl->_mouse_button_callback = callback;
}
void Window::set_cursor_pos_callback(CursorPosCallback callback) {
    impl->_cursor_pos_callback = callback;
}
void Window::set_fb_size_callback(FbSizeCallback callback) { impl->_fb_size_callback = callback; }

void Window::on_key(Key key, KeyAction action, KeyCallback callback) {
    impl->key_callbacks.insert({{key, action}, callback});
}

bool Window::toggle_vsync() {
    impl->vsync = !(impl->vsync);
    glfwSwapInterval(static_cast<int>(impl->vsync));
    return impl->vsync;
}

namespace {
std::optional<Key> int_to_key(int raw) {
    const std::array<int, 26> raw_keys{
        GLFW_KEY_A, GLFW_KEY_B, GLFW_KEY_C, GLFW_KEY_D, GLFW_KEY_E, GLFW_KEY_F, GLFW_KEY_G,
        GLFW_KEY_H, GLFW_KEY_I, GLFW_KEY_J, GLFW_KEY_K, GLFW_KEY_L, GLFW_KEY_M, GLFW_KEY_N,
        GLFW_KEY_O, GLFW_KEY_P, GLFW_KEY_Q, GLFW_KEY_R, GLFW_KEY_S, GLFW_KEY_T, GLFW_KEY_U,
        GLFW_KEY_V, GLFW_KEY_W, GLFW_KEY_X, GLFW_KEY_Y, GLFW_KEY_Z};
    const auto it = std::find(raw_keys.begin(), raw_keys.end(), raw);
    if (it == raw_keys.end()) {
        return std::nullopt;
    } else {
        const auto idx = std::distance(raw_keys.begin(), it);
        return static_cast<Key>(idx);
    }
}

std::optional<KeyAction> int_to_action(int raw) {
    const std::array<int, 26> raw_actions{
        GLFW_PRESS,
        GLFW_RELEASE,
    };
    const auto it = std::find(raw_actions.begin(), raw_actions.end(), raw);
    if (it == raw_actions.end()) {
        return std::nullopt;
    } else {
        const auto idx = std::distance(raw_actions.begin(), it);
        return static_cast<KeyAction>(idx);
    }
}
} // namespace
