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
        int m_destroying = 0;
        protected:
            friend class objLayer;

            Object();
            virtual ~Object();
        public:
            virtual void update() {}
            virtual void render() {}
        public:
            void destroy();
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
}

#endif

#endif