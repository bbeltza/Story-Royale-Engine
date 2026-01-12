#ifndef SRE_EVENT_HPP
#define SRE_EVENT_HPP

#include <Base/Signal.hpp>

namespace sre
{
	struct Event
	{

	};

	extern Signal<sre::Event> onEvent;
}

#endif