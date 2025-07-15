#pragma once
#include "Engine.h"

class Action
    {
        public:
        Action() = default;
        ~Action() = default;

        void AddInput(SDL_KeyCode keyCode) {m_keycodes.push_back(keyCode);}
        void AddInput(SDL_Scancode scanCode) {m_scancodes.push_back(scanCode);}
        void AddInput(InputClass::MouseButton mouseButton) {m_mousebuttons.push_back(mouseButton);}

        bool isPressed() {
            if (isKeyCodePressed() || isScanCodePressed() || isMousePressed())
                return true;
            already_pressed = false;
            return false;
        }
        bool isJustPressed() {
            if (isPressed() && !already_pressed)
                return already_pressed = true;
            return false;
        }

        private:
        bool already_pressed = false;

        std::vector<SDL_KeyCode> m_keycodes;
        std::vector<SDL_Scancode> m_scancodes;
        std::vector<InputClass::MouseButton> m_mousebuttons;

        bool isKeyCodePressed() const {for (auto keyCode : m_keycodes) if (Engine->Input.isKeyPressed(keyCode)) return true; return false;}
        bool isScanCodePressed() const {for (auto scanCode : m_scancodes) if (Engine->Input.isKeyPressed(scanCode)) return true; return false;}
        bool isMousePressed() const {for (auto mouseButton : m_mousebuttons) if (Engine->Input.isMouseButtonPressed(mouseButton)) return true; return false;}
    };