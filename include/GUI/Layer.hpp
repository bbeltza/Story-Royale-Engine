#ifndef SRE_GUI_LAYER_HPP
#define SRE_GUI_LAYER_HPP
#include <GUI/Object.hpp>
#include <deque>

#include <SDL_mutex.h>

namespace sreGUI
{
    Layer* get_default_layer();

    class Layer: public sre::Object
    {
        friend class sreGUI::Object;

        sre::ClipStackUT scissors_stack;
        std::deque<const sreGUI::Object*> hoveringstack;
        sreGUI::Object* root = NULL;

        sre::unit insets = 0;
    public: 
        Layer();
        ~Layer();
        void update() override;
        void render() override;

        const std::deque<const sreGUI::Object*>& get_hovering() { return hoveringstack; }

        sreGUI::Object* get_root() { return root; }
        void set_root(sreGUI::Object* object);

        sre::unit get_insets() { return insets; }
        void set_insets(sre::unit insets) { this->insets = insets; }
    };
}

#endif