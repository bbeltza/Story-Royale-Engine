#ifndef SREECS_SHAKE
#define SREECS_SHAKE

#include <ECS/Camera.hpp>
#include <Datatypes/TimeStamp.h>

namespace sreECS
{
	struct CameraShake: public CameraEffect
	{
		sre::vec2ut strength;

		sre::vec2ut on_process() override { turn = -turn; return { turn, turn }; }

		private:
			int turn = 3;
	};
}

#endif