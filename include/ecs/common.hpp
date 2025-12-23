#ifndef SREECS_COMMON_HPP
#define SREECS_COMMON_HPP

#include <Base/object.hpp>
#include <Base/signal.hpp>

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
        Signal<> updated{this};
        Signal<> rendered{this};
    };
}

#endif