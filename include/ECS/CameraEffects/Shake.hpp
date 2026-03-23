#ifndef SREECS_SHAKE
#define SREECS_SHAKE

#include <ECS/Camera.hpp>
#include <random>

namespace sreECS
{
	template <typename RandomNumberEngine=std::default_random_engine>
	struct CameraShakeT: public CameraEffect
	{
		using seed_type = typename RandomNumberEngine::result_type;
		static constexpr seed_type default_seed = RandomNumberEngine::default_seed; 

		sre::vec2ut strength;
		sre::timeStamp step_length = 0.02_ut;

		sre::vec2ut on_process(sre::timeStamp dt) override
		{
			m_time -= dt;
			if (m_time <= 0)
			{
				m_time = step_length;
				m_cache = sre::vec2ut{ m_dist(m_rng), m_dist(m_rng) } * strength;
			}
			return m_cache;
		}

		template <typename... Args>
		CameraShakeT(
			const sre::vec2ut& strength=10,
			sre::timeStamp step_len=0,
			seed_type seed=default_seed
		): strength(strength),
			step_length(step_len) {}

		void set_seed(seed_type seed) { m_rng.seed(seed); }
	private:
		sre::timeStamp m_time = 0;
		sre::vec2ut m_cache;

		RandomNumberEngine m_rng;
		std::uniform_real_distribution<sre::unit> m_dist{ -0.5_ut, 0.5_ut };
	};

	using CameraShake = CameraShakeT<>;
}

#endif