#ifndef SRE_OBJECT_HPP
#define SRE_OBJECT_HPP
#include <cstddef>

namespace sre
{
    class Object
    {
        friend class ECS;

        Object* m_nextdestroyed = nullptr;
        //Object* m_nextdestroyed = nullptr;
        // Reference count, destroy queues will forcefully be removed,
            // since what destroy queues solved can also be solved with ref-counting.
                // Ex: By incrementing the reference when updating an object, destroying that object inside an updade block won't crash since it has already been referenced so it won't get deleted until the end of the update where the reference count gets decremented back
        int m_ref = 0;
    protected:
        virtual ~Object();
    public:
        void destroy();
        int getref() const { return m_ref; }

        int reference();
        int release();
    };

    // NULL safe destroying
    inline void destroy(sre::Object* object) { if (object) object->destroy(); }
    // Object ref representing type (Wraps )
    template <class T>
    class ObjRef
    {
        sre::Object* m_ptr = NULL;

    public:
        constexpr ObjRef() = default;
        
        ObjRef(T* obj): m_ptr(obj) { static_cast<sre::Object*>(obj)->reference(); }
        ObjRef(T& obj): ObjRef(&obj) {}
        ~ObjRef() { release(); }

        ObjRef(const ObjRef& copy): m_ptr(copy.m_ptr) { if (m_ptr) m_ptr->reference(); }

        int getref() { return m_ptr ? m_ptr->getref() : 0; }
        int release()
        {
            if (!m_ptr)
            return 0;
            
            int value = m_ptr->release();
            m_ptr = nullptr;
            return value;
        }

    public:
        T* get() const { return static_cast<T*>(m_ptr); }
        //const T* get() const { return static_cast<T*>(m_ptr); }
        T* operator ->() const { return get(); }
        //const T* operator ->() const { return get(); }

        bool operator ==(const ObjRef& other) const { return m_ptr == other.m_ptr; }
    };
}

#endif