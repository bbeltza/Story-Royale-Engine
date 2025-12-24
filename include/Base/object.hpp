#ifndef SRE_OBJECT_HPP
#define SRE_OBJECT_HPP

#include <internal_def.hh>
#include <cstddef>

__def_internal(__destroy_queue);

class Connection;

namespace sre
{
    class Object
    {
        __friend_internal(__destroy_queue);

        Object* m_nextdestroyed = NULL;
    protected:
        virtual ~Object();
        virtual void on_destroy() { delete this; }
    public:
        void destroy();
    };
}

#endif