#pragma once

#include "GuiLayer.h"

namespace Game{
    namespace GuiPresets
    {
        class GuiButton: protected GuiObject
        {
        public:
            GuiButton();
            ~GuiButton();

            virtual void ButtonClick(MouseButton* Event) {}
        private:
            static std::vector<GuiButton*> s_buttons;
            static EVENT_CALLBACK_DECLARE(clickevent);
            static Connection* s_clickconnection;

            void Update(float dt) override;

            bool m_hover = 0;
        };
    }
}
