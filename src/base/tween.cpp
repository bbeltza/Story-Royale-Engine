#include <Core/Runtime.hpp>
#include <Base/Tween.hpp>

#include <utils/math.h>

using namespace sre;

using easingFn = timeStamp (timeStamp alpha);
static easingFn* EASE_FUNCTIONS[][3] = {
    /* S_LINEAR */ {
        [](timeStamp a) -> timeStamp { return a; },
        [](timeStamp a) -> timeStamp { return a; },
        [](timeStamp a) -> timeStamp { return a; }
    },
    /* S_QUAD */ {
        [](timeStamp a) -> timeStamp { return a*a; },
        [](timeStamp a) -> timeStamp { a = 1-a; return 1 - a*a; },
        [](timeStamp a) -> timeStamp { return a < 0.5_ts ? 2 * a*a : static_cast<timeStamp>(1 - pow(-2 * a + 2, 2)/2 ); }
    },
    /* S_CUBIC  */ {
        [](timeStamp a) -> timeStamp { return a*a*a; },
        [](timeStamp a) -> timeStamp { a = 1-a; return 1 - a*a*a; },
        [](timeStamp a) -> timeStamp { return a < 0.5_ts ? 4 * a*a*a : static_cast < timeStamp>(1 - pow(-2 * a + 2, 3)/2 ); }
    }
};

void TweenServer::update()
{
    for (size_t i = 0; i < m_tweens.size(); i++)
    {
        auto& tw = m_tweens.at(i);

        if (!tw->m_state)
        {
            if (tw.unique())
            {
                tw.swap(m_tweens.back());
                m_tweens.pop_back();
                i--;
            }
            continue;
        }
        const timeStamp duration = tw->style.duration;

        tw->m_elapsed += sre::dt;

        sre_easingStyle es = tw->style.easing_style;
        sre_easingDirection ed = tw->style.easing_direction;
        easingFn* easefunc = EASE_FUNCTIONS[es][ed];
        assert(easefunc != NULL);

        timeStamp alpha = duration ? tw->m_elapsed / duration : 1;
        ut_setclamp(alpha, 0.0_ts, 1.0_ts);

        alpha = easefunc(alpha);

        tw->step(alpha);

        if (tw->m_elapsed >= duration)
        {
            tw->m_state = 0;
            tw->m_elapsed = 0;
            tw->completed.fire();
        }
    }
}