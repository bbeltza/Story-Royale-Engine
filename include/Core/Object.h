#ifndef SRE_OBJECT_HPP
#define SRE_OBJECT_HPP

#include <C_API.h>
#include <stddef.h>
#include <stdint.h>

SRE_CAPI_BEGIN

typedef struct sre_Object sre_Object;

// Pipeline object table
typedef struct sre_ObjectTable
{
    void (*destroy)(void* object);
    void (*update)(void* object);
    void (*render)(void* object);
} sre_ObjectTable;

sre_Object* sre_create_object(const sre_ObjectTable* functiontable, size_t extrasize);
void sre_destroy_object(sre_Object* object);

void* sre_get_object_data(sre_Object* object);

// C equivalents of sre::bind_object_layer
int sre_bind_objectlayer(sre_Object* object);
void sre_unbind_objectlayer(int index);

SRE_CAPI_END

#ifdef __cplusplus
#include <new>
#include <atomic>

namespace sre
{
    class Object
    {
        enum _state {
            _STATE_NORMAL = 0,
            _STATE_DESTROYING,
            _STATE_STATIC
        };

        _state m_state = _STATE_NORMAL;
        protected:
            friend class objLayer;

            Object();
            virtual ~Object();
        public:
            virtual void update() {}
            virtual void render() {}
        public:
            void destroy();
            bool is_static() const { return m_state == _STATE_STATIC; }

        public:
            // Don't call destroy the object whenever `destroy` gets called. Make it static,
            //  used to tell that an object has automatic duration. Usually, you'd call `new` on it
            //  and call it a day, but you don't always want heap allocations everywhere... So calling
            //  this function determines that, you might also want to look at the sre::static_obj template
            //  which does that for you automatically...
            // Calling `destroy` on an object and then calling __make_static is undefined behavior.
            // You should not consider using this function at all...
            static void __make_static(sre::Object& obj) {
                /*
                if (obj.m_state != _STATE_NORMAL)
                    return;
                */
                obj.m_state = _STATE_STATIC;
            }
    };

    inline void safe_destroy(sre::Object*& obj) {
        if (!obj)
            return;
        
        obj->destroy();
        obj = NULL;
    }

    // Bind an object to the core engine's pipeline layer
    // This function may not be used by the user, since it's used by ECS and GUI layers to properly function, creating a static context that holds the current layers.
    // But if you want to create a separate layer that runs independently from the ECS and GUI layer (maybe have a layer that updates on another window once it'll be available), then use this function
    // @param object The object to be bound to the pipeline. Remind that you may not use an ECS or GUI object here.
    // @returns The index of the bound object. Aka the order in which the engine will look into the object
    int bind_object_layer(sre::Object& object);
    void unbind_object_layer(int index);

    // Static object class template (experimental)
    // Use this if you don't want extra heap allocations over an object, but know that that object is going to live within a scope or struct's lifetime.
    // Other than that, it works just like a pointer to the object (with its overloaded operators)
    template <typename T=sre::Object>
    class static_obj
    {
        /* Is this, uhhh... necessary?¿?
        using value_type = T;
        using pointer_type = T*;
        */

        T m_obj;
    public:
        template <typename... Args>
        static_obj(Args&&... args): m_obj(std::forward<Args>(args)...)  {
            sre::Object::__make_static(m_obj);
        }

        const T* operator ->() const { return &m_obj; }
              T* operator ->()       { return &m_obj; }
    
        // Should just return the exact same address as the `static_obj` type
        const T* operator &() const { return &m_obj; }
              T* operator &()       { return &m_obj; }

        const T& operator *() const { return m_obj; }
              T& operator *()       { return m_obj; }

    public:
        const T* ptr() const { return &m_obj; }
              T* ptr()       { return &m_obj; }

        const T& ref() const { return m_obj; }
              T& ref()       { return m_obj; }
    };
}

#endif

#endif