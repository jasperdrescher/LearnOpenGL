#pragma once

#include <map>
#include <vector>

enum class Keys
{
    Undefined,
    Spacebar,
    Apostrophe,
    Comma,
    Minus,
    Period,
    Slash,
    Zero,
    One,
    Two,
    Three,
    Four,
    Five,
    Six,
    Seven,
    Eight,
    Nine,
    Semicolon,
    Equal,
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    LeftBracket,
    Backslash,
    RightBracket,
    GraveAccent,
    Escape,
    Enter,
    Tab,
    Backspace,
    Insert,
    Delete,
    Right,
    Left,
    Down,
    Up,
    PageUp,
    PageDown,
    Home,
    End,
    CapsLock,
    ScrollLock,
    NumLock,
    PrintScreen,
    Pause,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    F13,
    F14,
    F15,
    F16,
    F17,
    F18,
    F19,
    F20,
    F21,
    F22,
    F23,
    F24,
    F25,
    KeypadZero,
    KeypadOne,
    KeypadTwo,
    KeypadThree,
    KeypadFour,
    KeypadFive,
    KeypadSix,
    KeypadSeven,
    KeypadEight,
    KeypadNine,
    KeypadDecimal,
    KeypadDivide,
    KeypadMultiply,
    KeypadSubtract,
    KeypadAdd,
    KeypadEnter,
    KeypadEqual,
    LeftShift,
    LeftControl,
    LeftAlt,
    LeftSuper,
    RightShift,
    RightControl,
    RightAlt,
    RightSuper,
    Menu
};

enum class MouseButtons
{
    Undefined,
    Left,
    Right,
    Middle
};

class InputManager
{
public:
    static InputManager& GetInstance()
    {
        static InputManager instance;
        return instance;
    }

    InputManager(InputManager const&) = delete;
    void operator=(InputManager const&) = delete;

    void ClearKeyActionsThisFrame();
    void OnKeyAction(int aKey, int, bool aIsKeyDown, int);
    void OnCursorAction(double aXPosition, double aYPosition);
    void OnScrollAction(double aXOffset, double aYOffset);
    void OnMouseButtonAction(int aButton, int anAction, int aModifier);

    [[nodiscard]] bool IsKeyDown(Keys aKey) const;
    [[nodiscard]] bool IsMouseButtonDown(MouseButtons aMouseButton) const;
    [[nodiscard]] const std::vector<Keys>& GetReleasedKeysThisFrame() const { return myReleasedKeysThisFrame; }
    [[nodiscard]] const std::vector<Keys>& GetPressedKeysThisFrame() const { return myPressedKeysThisFrame; }
    [[nodiscard]] float GetCursorXPosition() const { return myCursorXPosition; }
    [[nodiscard]] float GetCursorYPosition() const { return myCursorYPosition; }
    [[nodiscard]] float GetScrollXOffset() const { return myScrollXOffset; }
    [[nodiscard]] float GetScrollYOffset() const { return myScrollYOffset; }

private:
    InputManager();
    ~InputManager() = default;

    static Keys GetTranslatedKey(int aKey);
    static MouseButtons GetTranslatedMouseButton(int aButton);

    std::map<Keys, bool> myKeys;
    std::map<MouseButtons, bool> myMouseButtons;
    std::vector<Keys> myReleasedKeysThisFrame;
    std::vector<Keys> myPressedKeysThisFrame;
    float myCursorXPosition;
    float myCursorYPosition;
    float myScrollXOffset;
    float myScrollYOffset;
};