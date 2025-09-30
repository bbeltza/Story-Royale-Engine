#pragma once
#include "Engine.hpp"

class Action
    {
        public:
        Action() = default;
        ~Action() = default;

        inline void AddInput(SDL_KeyCode keyCode) {m_keycodes.insert(keyCode);}
        inline void AddInput(SDL_Scancode scanCode) {m_scancodes.insert(scanCode);}
        inline void AddInput(InputClass::MouseButton mouseButton) {m_mousebuttons.insert(mouseButton);}

        inline bool isPressed() {
            if (isKeyCodePressed() || isScanCodePressed() || isMousePressed())
                return true;
            press_frame = -1;
            return false;
        }
        inline bool isJustPressed() {
            if (isPressed() && press_frame < 0)
                return (press_frame = Engine->runtime_frame()) || true;
            
            return press_frame == Engine->runtime_frame();
        }

        private:
        long long press_frame = -1;

        std::unordered_set<SDL_KeyCode> m_keycodes;
        std::unordered_set<SDL_Scancode> m_scancodes;
        std::unordered_set<InputClass::MouseButton> m_mousebuttons;

        inline bool isKeyCodePressed() const {for (auto keyCode : m_keycodes) if (Engine->Input.isKeyPressed(keyCode)) return true; return false;}
        inline bool isScanCodePressed() const {for (auto scanCode : m_scancodes) if (Engine->Input.isKeyPressed(scanCode)) return true; return false;}
        inline bool isMousePressed() const {for (auto mouseButton : m_mousebuttons) if (Engine->Input.isMouseButtonPressed(mouseButton)) return true; return false;}
    };