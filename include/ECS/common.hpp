#ifndef SREECS_COMMON_HPP
#define SREECS_COMMON_HPP

#include <Core/object.hpp>
#include <Base/Signal.hpp>

namespace sreECS
{
    class Common: public sre::Object
    {
    protected:
        virtual void update() {}
        virtual void pupdate() {}
        virtual void pre_render() {}
        virtual void post_render() {}
    public:
        sre::Signal<sre::empty_t> updated{this};
        sre::Signal<sre::empty_t> rendered{this};
    };
}

#endif
