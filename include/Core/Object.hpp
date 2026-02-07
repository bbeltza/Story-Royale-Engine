#ifndef SRE_OBJECT_HPP
#define SRE_OBJECT_HPP

namespace sre
{
    class Object
    {
        friend class ECS;

        Object* m_nextdestroyed = nullptr;
    protected:
        virtual ~Object();
    public:
        void destroy();
    };
}

#endif