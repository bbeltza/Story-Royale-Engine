#ifndef SRE_OBJECT_HPP
#define SRE_OBJECT_HPP

#include <internal_def.hh>
#include <cstddef>
#include <string>

__def_internal(__destroy_queue);

namespace sre
{
    class Object
    {
        __friend_internal(__destroy_queue);

        Object* m_nextdestroyed = NULL;
    protected:
        virtual ~Object();
    public:
        void destroy();
    };
}

#endif