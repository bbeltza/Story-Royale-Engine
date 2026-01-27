#include <Core/Runtime.hpp>
#include <Base/Tween.hpp>

#include <utils/math.h>

using namespace sre;

void TweenServer::update()
{
    for (size_t i = 0; i < m_tweens.size(); i++)
    {
        auto& tw = m_tweens.at(i);

        if (!tw->m_state) continue;
        const timeStamp duration = tw->style.duration;

        tw->m_elapsed += sre::dt;
        
        timeStamp alpha = duration ? tw->m_elapsed / duration : 1;
        ut_setclamp(alpha, 0.0_ts, 1.0_ts);
        tw->step(alpha);

        if (tw->m_elapsed >= duration)
        {
            tw->m_state = 0;
            tw->completed.fire();

            m_tweens.at(i).swap(m_tweens.back());
            m_tweens.pop_back();
            i--;
        }
    }
}