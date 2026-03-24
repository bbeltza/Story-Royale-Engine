#include "../internal.h"
#include "drivers/drivers.h"

#include <Core/Event.hpp>

// So since this is the only cpp source that has access to every single event, I'm processing ImGui's events here!
    #include <backends/imgui_impl_sdl2.h>
//

namespace sre
{
    using EventQueue = std::deque<sre::Event>;
    using EventMutex = std::mutex;
    using EventGuard = std::lock_guard<EventMutex>;
}

static sre::EventQueue queue;
static sre::EventMutex mutex;
sre::Signal<sre::Event> sre::onEvent;

int __event_filter(void* userdata, SDL_Event* ev)
{
    #ifndef IMGUI_DISABLE
    if (ev->type == SDL_QUIT) return 1;
    if (engine.video && engine.video->imgui)
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGui_ImplSDL2_ProcessEvent(ev);
        if (io.WantCaptureKeyboard)
            return 0;
    }
    #endif
    return 1;
}

int __signal_events(SDL_Event* ev)
{
    sre::Event current;

    switch (ev->type)
    {
    case SDL_KEYUP:
    case SDL_KEYDOWN:
        current.type = sre::EVENT_KEYPRESS;
        current.key_press.scancode = static_cast<sre::scanCode>(ev->key.keysym.scancode);
        current.key_press.keycode = static_cast<sre::keyCode>(ev->key.keysym.sym);
        current.key_press.keymod = static_cast<sre::keyMod>(ev->key.keysym.mod);

        current.key_press.press = ev->key.state + ev->key.repeat;
        break;
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEBUTTONDOWN:
        current.type = sre::EVENT_MOUSEBUTTON;
        current.mouse_button.id = ev->button.which;
        current.mouse_button.button = ev->button.button;
        current.mouse_button.clicks = ev->button.clicks;
        current.mouse_button.pressed = ev->button.state == SDL_PRESSED;
        current.mouse_button.position = sre::vec2ut{ ev->button.x, ev->button.y } * engine.scale_ratio;
        break;
    case SDL_MOUSEMOTION:
        current.type = sre::EVENT_MOUSEMOVE;
        current.mouse_move.id = ev->motion.which;
        current.mouse_move.button_state = ev->motion.state;
        current.mouse_move.position = sre::vec2ut{ ev->motion.x, ev->motion.y } * engine.scale_ratio;
        current.mouse_move.delta = sre::vec2ut{ ev->motion.xrel, ev->motion.yrel } * engine.scale_ratio;
        break;
    case SDL_MOUSEWHEEL:
        current.type = sre::EVENT_MOUSEWHEEL;
        current.mouse_wheel.id = ev->wheel.which;
        current.mouse_wheel.position = sre::vec2ut{ ev->wheel.mouseX, ev->wheel.mouseY } * engine.scale_ratio;
        current.mouse_wheel.amount = sre::vec2i{ ev->wheel.x, ev->wheel.y };
        break;        
    case SDL_FINGERUP:
    case SDL_FINGERDOWN:
    case SDL_FINGERMOTION:
        current.type = sre::EVENT_TOUCH;
        current.touch.uv = sre::vec2ut{ ev->tfinger.x, ev->tfinger.y };
        current.touch.delta = sre::vec2ut{ ev->tfinger.dx, ev->tfinger.dy };
        current.touch.pressure = ev->tfinger.pressure;
        current.touch.pressed = ev->type != SDL_FINGERUP;
        break;
    default:
        return 1;
    }

    sre::EventGuard guard{mutex};
    queue.push_back(std::move(current));

    return 1;
}

void __queue_events()
{
    memset(&engine.keyboard_framestate, 0, sizeof(engine.keyboard_framestate));
    memset(&engine.mouse_framepress, 0, sizeof(engine.mouse_framepress));
    engine.lastfingerup = 0;

    for (auto& ev : queue )
    {
        switch (ev.type)
        {
            case sre::EVENT_MOUSEBUTTON:
                if (!ev.mouse_button.pressed)
                    break;
            
                engine.mouse_framepress |= SDL_BUTTON(ev.mouse_button.button);
                break;
            case sre::EVENT_KEYPRESS:
                if (ev.key_press.press != sre::KEY_PRESSED)
                    break;
                
                engine.keyboard_framestate[ev.key_press.scancode/8] |= (1 << (ev.key_press.scancode%8));
                break;
            case sre::EVENT_TOUCH:
                if (ev.touch.pressed)
                    break;

                engine.lastfingerup <<= 1;
                engine.lastfingerup |= 1;
            default:
                break;
        }
    }

    while (!queue.empty())
    {
        mutex.lock();
            sre::Event ev = std::move(queue.front());
            queue.pop_front();
        mutex.unlock();
        // Don't forget to fire with the mutex unlocked, having the mutex locked can cause some softlocks
        sre::onEvent.fire(ev);
    }
}