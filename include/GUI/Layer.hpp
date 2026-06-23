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

        sre::rect2Dut vp_area = { 0, 0 };
        sre::unit vp_scale = 0;
        sre::unit insets = 0;

        sre::vec2ut _displaycache; // Cache value that is returned from sre::calc_viewport_size(), to reduce its calls
    public: 
        Layer();
        ~Layer();
        void update() override;
        void render() override;

        const std::deque<const sreGUI::Object*>& get_hovering() { return hoveringstack; }

        sreGUI::Object* get_root() { return root; }
        void set_root(sreGUI::Object* object);

        sre::unit get_insets() { return insets; }
        sre::unit get_viewport_scale() { return vp_scale; } // This will return 0 if the scale is set to 0 (the default)! You may want to call sre::window_getscale() to get the real scale
        sre::rect2Dut get_viewport_area() { return vp_area; }

        void set_insets(sre::unit insets) { this->insets = insets; }
        void set_viewport(sre::rect2Dut area, sre::unit scale=0) {
            vp_area = area;
            vp_scale = scale;
        }
    };
}

#endif