#pragma once

#include "GuiLayer.h"
#include "GuiComponents.h"

namespace Game{
    namespace GuiPresets
    {
        class GuiButton: protected GuiObject
        {
        public:
            GuiButton();
            ~GuiButton();

            virtual void ButtonClick(MouseButton* Event) {}
            virtual void MouseHover(bool hovered) {}
        private:
            static std::list<GuiButton*> s_buttons;
            static void clickevent(MouseButton* buttonData);
            static Connection* s_clickconnection;

            void Update(float dt) override;

            bool m_hover = 0;
            GuiComponents::UIModulate* m_mod = pushGuiComponent<GuiComponents::UIModulate>();
        };
    }
}
