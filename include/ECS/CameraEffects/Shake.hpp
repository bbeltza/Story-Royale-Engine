#ifndef SREECS_SHAKE
#define SREECS_SHAKE

#include <ECS/Camera.hpp>

namespace sreECS
{
	struct CameraShake: public CameraEffect
	{
		sre::vec2ut strength;
		sre::timeStamp duration = 0;

		sre::vec2ut on_process(sre::timeStamp dt) override
		{
			m_time -= dt;
			if (m_time <= 0)
			{
				m_time = duration;
				m_cache = sre::vec2ut{ rand(), rand() } / RAND_MAX * strength;
			}
			return m_cache;
		}

		CameraShake(
			const sre::vec2ut& strength=10
		): strength(strength) {}

	private:
		sre::timeStamp m_time = 0;
		sre::vec2ut m_cache;
	};
}

#endif