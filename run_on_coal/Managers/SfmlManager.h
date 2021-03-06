#pragma once

namespace ROC
{

class Core;
class LuaArguments;
typedef void(*OnWindowResizeCallback)(unsigned int, unsigned int);
typedef void(*OnWindowFocusCallback)(bool);
typedef void(*OnKeyPressCallback)(int, bool);
typedef void(*OnTextInputCallback)(const std::string&);
typedef void(*OnMouseMoveCallback)(int, int);
typedef void(*OnCursorEnterCallback)(bool);
typedef void(*OnMouseKeyPressCallback)(int, bool);
typedef void(*OnMouseScrollCallback)(int, float);
typedef void(*OnJoypadStateChangeCallback)(unsigned int, bool);
typedef void(*OnJoypadButtonCallback)(unsigned int, unsigned int, bool);
typedef void(*OnJoypadAxisCallback)(unsigned int, int, float);

class SfmlManager final
{
    Core *m_core;

    sf::VideoMode m_windowVideoMode;
    sf::ContextSettings m_contextSettings;
    sf::Uint32 m_windowStyle;
    unsigned int m_frameLimit;
    sf::Window *m_window;
    sf::Event m_event;
    bool m_active;

    sf::Clock m_clock;
    float m_time;

    LuaArguments *m_argument;

    OnWindowResizeCallback m_windowResizeCallback;
    OnWindowFocusCallback m_windowFocusCallback;
    OnKeyPressCallback m_keyPressCallback;
    OnTextInputCallback m_textInputCallback;
    OnMouseMoveCallback m_mouseMoveCallback;
    OnCursorEnterCallback m_cursorEnterCallback;
    OnMouseKeyPressCallback m_mouseKeyPressCallback;
    OnMouseScrollCallback m_mouseScrollCallback;
    OnJoypadStateChangeCallback m_joypadStateChangeCallback;
    OnJoypadButtonCallback m_joypadButtonCallback;
    OnJoypadAxisCallback m_joypadAxisCallback;

    SfmlManager(const SfmlManager& that);
    SfmlManager &operator =(const SfmlManager &that);
public:
    void GetWindowPosition(glm::ivec2 &f_pos);
    void SetWindowPosition(const glm::ivec2 &f_pos);
    void GetWindowSize(glm::ivec2 &f_size);
    inline void CloseWindow() { m_active = false; }
    inline void SetVSync(bool f_sync) { m_window->setVerticalSyncEnabled(f_sync); }
    void SetFramelimit(unsigned int f_fps);
    inline unsigned int GetFramelimit() const { return m_frameLimit; }
    inline void SetTitle(const sf::String &f_title) { m_window->setTitle(f_title); }
    bool SetIcon(const std::string &f_path);
    inline void RequestFocus() { m_window->requestFocus(); }
    inline bool GetFocusState() const { return m_window->hasFocus(); }

    void SetCursorMode(bool f_visible, bool f_lock);
    void GetCursorPosition(glm::ivec2 &f_pos);
    void SetCursorPosition(const glm::ivec2 &f_pos);

    static bool IsKeyPressed(int f_key);
    static bool IsMouseKeyPressed(int f_key);

    static bool IsJoypadConnected(unsigned int f_jp);
    static bool GetJoypadButtonState(unsigned int f_jp, unsigned int f_button);
    static unsigned int GetJoypadButtonCount(unsigned int f_jp);
    static bool CheckJoypadAxis(unsigned int f_jp, unsigned int f_axis);
    static float GetJoypadAxisValue(unsigned int f_jp, unsigned int f_axis);

    inline float GetTime() const { return m_time; }

    inline void SetWindowResizeCallback(OnWindowResizeCallback f_callback) { m_windowResizeCallback = f_callback; }
    inline void SetWindowFocusCallback(OnWindowFocusCallback f_callback) { m_windowFocusCallback = f_callback; }
    inline void SetKeyPressCallback(OnKeyPressCallback f_callback) { m_keyPressCallback = f_callback; }
    inline void SetTextInputCallback(OnTextInputCallback f_callback) { m_textInputCallback = f_callback; }
    inline void SetMouseMoveCallback(OnMouseMoveCallback f_callback) { m_mouseMoveCallback = f_callback; }
    inline void SetCursorEnterCallback(OnCursorEnterCallback f_callback) { m_cursorEnterCallback = f_callback; }
    inline void SetMouseKeyPressCallback(OnMouseKeyPressCallback f_callback) { m_mouseKeyPressCallback = f_callback; }
    inline void SetMouseScrollCallback(OnMouseScrollCallback f_callback) { m_mouseScrollCallback = f_callback; }
    inline void SetJoypadStateChangeCallback(OnJoypadStateChangeCallback f_callback) { m_joypadStateChangeCallback = f_callback; }
    inline void SetJoypadButtonCallback(OnJoypadButtonCallback f_callback) { m_joypadButtonCallback = f_callback; }
    inline void SetJoypadAxisCallback(OnJoypadAxisCallback f_callback) { m_joypadAxisCallback = f_callback; }
protected:
    explicit SfmlManager(Core *f_core);
    ~SfmlManager();

    inline void SwapBuffers() { m_window->display(); }

    bool DoPulse();

    friend class Core;
    friend class RenderManager;
};

}
