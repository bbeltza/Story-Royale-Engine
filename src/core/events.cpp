#include "../internal.h"

#include <Core/Event.hpp>
#include <Base/Mutex.hpp>

sre::vec2ut sre::process_input_coordinates(sre::vec2ut coords, sre::vec2ut origin, sre::unit scale) {
    const sre::unit ratio = scale ? 1/scale : engine.scale_ratio;    
    return coords*ratio - origin;
}

namespace sre
{
    using EventQueue = std::deque<sre::Event>;

    struct QuitEventDummy {
        static constexpr eventType EVENT_TYPE = sre::EVENT_QUIT;
    };
}

static sre::EventQueue queue;
static sre::Mutex mutex;
sre::Signal<sre::Event> sre::onEvent;

int __signal_events(SDL_Event* ev)
{
    std::lock_guard<sre::Mutex> guard{mutex};

    switch (ev->type)
    {
    using namespace sre::events;
    case SDL_KEYUP:
    case SDL_KEYDOWN:
        queue.emplace_back(Key{
            static_cast<sre::keyCode>(ev->key.keysym.sym),
            static_cast<sre::scanCode>(ev->key.keysym.scancode),
            static_cast<sre::keyMod>(ev->key.keysym.mod),

            ev->key.state + ev->key.repeat
        });
        break;
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEBUTTONDOWN:
        queue.emplace_back(MouseButton{
            ev->button.which,
            ev->button.button,
            ev->button.clicks,

            ev->button.state == SDL_PRESSED,

            sre::vec2i{ ev->button.x, ev->button.y }
        });
        break;
    case SDL_MOUSEMOTION:
        queue.emplace_back(MouseMove{
            ev->button.which,
            ev->motion.state,
            
            sre::vec2i{ ev->motion.x, ev->motion.y },
            sre::vec2i{ ev->motion.xrel, ev->motion.yrel }
        });
        break;
    case SDL_MOUSEWHEEL:
        queue.emplace_back(MouseWheel{
            ev->button.which,
            
            sre::vec2i{ ev->wheel.x, ev->wheel.y },
            sre::vec2i{ ev->wheel.mouseX, ev->wheel.mouseY }
        });
        break;        
    case SDL_FINGERUP:
    case SDL_FINGERDOWN:
    case SDL_FINGERMOTION:
        queue.emplace_back(Touch{
            sre::vec2ut{ ev->tfinger.x, ev->tfinger.y } * engine.osize,
            sre::vec2ut{ ev->tfinger.dx, ev->tfinger.dy } * engine.osize,

            ev->tfinger.pressure,
            ev->type != SDL_FINGERUP
        });
        break;
    
    case SDL_TEXTINPUT: {
        unsigned int codepoint = 0;
        unsigned char const* utext = reinterpret_cast<const unsigned char*>(ev->text.text);
        char utf8[4]{};
        /* Handle utf-8 codepoint (Should be moved into a different headers, specially when more unicode support will be required, with font UnicodeSet s for example...) */
        /* Most of the calculations were figured out thanks to the list of unicode codepoints. (I use a spanish keyboard which has non-ascii keys) */
        if (ev->text.text[0] > 0) {
            codepoint = utext[0];
            utf8[0] = ev->text.text[0];
        }
        else if ((ev->text.text[0] & 0xE0) == 0xC0) {
            codepoint = (utext[1] & ~0xC0) | ((utext[0] & ~0xE0) << 6);
            utf8[0] = ev->text.text[0];
            utf8[1] = ev->text.text[1];
        }
        else if ((ev->text.text[0] & 0xF0) == 0xE0) {
            codepoint = (utext[2] & ~0xC0) | ((utext[1] & ~0xC0) << 6) | ((utext[0] & ~0xF0) << (6+6));
            utf8[0] = ev->text.text[0];
            utf8[1] = ev->text.text[1];
            utf8[2] = ev->text.text[2];
        }
        else if ((ev->text.text[0] & 0xF8) == 0xF0) {
            codepoint = (utext[3] & ~0xC0) | ((utext[2] & ~0xC0) << 6) | ((utext[1] & ~0xC0) << (6+6)) | ((utext[0] & ~0xF8) << (6+6+6));
            utf8[0] = ev->text.text[0];
            utf8[1] = ev->text.text[1];
            utf8[2] = ev->text.text[2];
            utf8[3] = ev->text.text[3];
        }

        queue.emplace_back(TextInput{codepoint, {utf8[0], utf8[1], utf8[2], utf8[3]}});
    } break;
    case SDL_QUIT:
        queue.emplace_back(sre::QuitEventDummy{});
        break;    
    default:
        break;
    }
    
    return 1;
}

void __queue_events()
{
    memset(&engine.keyboard_framestate, 0, sizeof(engine.keyboard_framestate));
    memset(&engine.mouse_framepress, 0, sizeof(engine.mouse_framepress));
    engine.lastfingerup = 0;

    for (auto& ev : queue )
    {
        switch (ev.type())
        {
            using namespace sre::events;
            case sre::EVENT_MOUSEBUTTON:
                if (!ev.get<MouseButton>().pressed)
                    break;
            
                engine.mouse_framepress |= SDL_BUTTON(ev.get<MouseButton>().button);
                break;
            case sre::EVENT_KEYPRESS:
                if (ev.get<Key>().press != sre::KEY_PRESSED)
                    break;
                
                engine.keyboard_framestate[ev.get<Key>().scancode/8] |= (1 << (ev.get<Key>().scancode%8));
                break;
            case sre::EVENT_TOUCH:
                if (ev.get<Touch>().pressed)
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
        // Don't forget to fire with the mutex unlocked, having the mutex locked can cause bad locks
        sre::onEvent.fire(ev);
    }
}