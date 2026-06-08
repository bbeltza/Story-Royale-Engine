#include <Core/Render.h>
#include <Core/Object.h>
#include <Core/Display.hpp>
#include <Core/Runtime.hpp>
#include <Base/Log.h>

#include <Hints.h>

#include "../internal.h"

#include <cassert>

#define OBJLAYER_TOVECTOR() reinterpret_cast<std::vector<sre::Object*>*>(engine._obj_layer_vdata)
#define OBJLAYERFL_TOVECTOR() reinterpret_cast<std::vector<int>*>(engine._obj_layerfl_vdata)
#define OBJLAYER_STARTCAPACITY 16

#define DQ_LOCK() SDL_LockMutex(sre::DESTROY_QUEUE_MUTEX)
#define DQ_UNLOCK() SDL_UnlockMutex(sre::DESTROY_QUEUE_MUTEX)

#define SRE_ENABLE_OBJECT_TRACKER 0

namespace sre
{
    #if SRE_ENABLE_OBJECT_TRACKER
        std::list<sre::Object*> OBJECT_TRACKER;
    #endif


    std::queue<sre::Object*> DESTROY_QUEUE;
    SDL_mutex* DESTROY_QUEUE_MUTEX;
    bool IS_IN_DESTROYQUEUE = false;

    class objLayer
    {
        public:
            static void delete_object(sre::Object* obj) { delete obj; }
    };

    void __flush_destroy_queue() {
        DQ_LOCK();
        IS_IN_DESTROYQUEUE = true;

        while (!sre::DESTROY_QUEUE.empty()) {
            sre::objLayer::delete_object(sre::DESTROY_QUEUE.front());
            sre::DESTROY_QUEUE.pop();
        }

        IS_IN_DESTROYQUEUE = false;
        DQ_UNLOCK();
    }
}

bool __update_objects()
{
    auto& obj_layer = *OBJLAYER_TOVECTOR();
    for (auto obj : obj_layer)
        obj->update();

    sre::onUpdate.fire();
    sre::__flush_destroy_queue();

    if (SDL_GetWindowFlags(engine.sdl_windowhndl) & SDL_WINDOW_SHOWN)
    {
        sre::beforeRender.fire();
        for (auto obj : obj_layer)
            obj->render();
        
        sre::afterRender.fire();

        #if SDL_VIDEO_DRIVER_WAYLAND // Wayland doesn't show up windows unless it's getting presented
            if (!sre::render::has_begun())
                sre::render::begin(sre::BLACK, sre::vec2ut::ZERO);
        #endif
        return true;
    }

    return false;
}

void __setup_objects()
{
    new(engine._obj_layer_vdata) std::vector<sre::Object*>();
    new(engine._obj_layerfl_vdata) std::vector<int>();
    OBJLAYER_TOVECTOR()->reserve(OBJLAYER_STARTCAPACITY); 
    OBJLAYERFL_TOVECTOR()->reserve(OBJLAYER_STARTCAPACITY);

    sre::DESTROY_QUEUE_MUTEX = SDL_CreateMutex();

    using sreECS_entry = void(*)(void);

    auto ecs_entry = reinterpret_cast<const sreECS_entry>(sre::gethint("ECS_ENTRYPOINT"));
    auto gui_entry = reinterpret_cast<const sreECS_entry>(sre::gethint("GUI_ENTRYPOINT"));

    if (ecs_entry)
        ecs_entry();
    if (gui_entry)
        gui_entry();

    #if SRE_ENABLE_OBJECT_TRACKER
        sre::log("Objects currently living after `__setup_objects()`: %zu", sre::OBJECT_TRACKER.size());
    #endif
}

void __cleanup_objects()
{
    auto& obj_layer = *OBJLAYER_TOVECTOR();
    auto& obj_layerfl = *OBJLAYERFL_TOVECTOR();
    for (auto obj : obj_layer)
        sre::objLayer::delete_object(obj);
    
    obj_layer.~vector();
    obj_layerfl.~vector();
    memset(engine._obj_layer_vdata, 0, sizeof(engine._obj_layer_vdata));
    sre::__flush_destroy_queue();
    
    SDL_DestroyMutex(sre::DESTROY_QUEUE_MUTEX);
    sre::DESTROY_QUEUE_MUTEX = NULL;
    
    #if SRE_ENABLE_OBJECT_TRACKER
        sre::log("Objects living currently after `__cleanup_objects()`: %zu", sre::OBJECT_TRACKER.size());
        for (auto& obj : sre::OBJECT_TRACKER)
        {
            sre::log("Object still living at %p after `__cleanup_objects()` ; This is known due to an object tracker enabled inside this engine, however, it won't destroy any of the remaining objects. So this is a memory leak", obj);
        }
    #endif
}

// Public API

    // Not much to do on those functions

sre::Object::Object()
{
    #if SRE_ENABLE_OBJECT_TRACKER
        OBJECT_TRACKER.push_back(this);
        //if (OBJECT_TRACKER.size() == xx) SDL_TriggerBreakpoint();
        sre::log("Adding new object (%p)... Current count: %zu", this, OBJECT_TRACKER.size());
    #endif
}

sre::Object::~Object()
{
    #if SRE_ENABLE_OBJECT_TRACKER
        OBJECT_TRACKER.remove(this);
        sre::log("Removing existing object (%p)... Current count: %zu", this, OBJECT_TRACKER.size());
    #endif
}

void sre::Object::destroy()
{
    if (m_destroying)
        return;

    DQ_LOCK();

    if (IS_IN_DESTROYQUEUE)
    {
        delete this; // Already queueing object destruction. Commit suicide!
        goto FINISH;
    }

    m_destroying = true;
    sre::DESTROY_QUEUE.push(this);

FINISH:
    DQ_UNLOCK();
}

static void* ZERO_DATA_CMP[4];
#define INIT_CHECK() assert(memcmp(engine._obj_layer_vdata, ZERO_DATA_CMP, sizeof(ZERO_DATA_CMP)) != 0 && "Attempt to call sre::bind_object_layer before or after engine's lifetime") 
int sre::bind_object_layer(sre::Object& object)
{
    INIT_CHECK();

    auto& objects = *OBJLAYER_TOVECTOR();
    auto& freelist = *OBJLAYERFL_TOVECTOR();
    if (freelist.empty())
    {
        int pos = static_cast<int>(objects.size());
        objects.push_back(&object);
        return pos;
    }
    else
    {
        int pos = freelist.back();
        freelist.pop_back();
        assert(objects.at(pos) == NULL);
        objects.at(pos) = &object;
        return pos;
    }
}
void sre::unbind_object_layer(int index)
{
    if (index < 0)
        return;

    auto& objects = *OBJLAYER_TOVECTOR();
    auto& freelist = *OBJLAYERFL_TOVECTOR();
    if (objects.at(index) == NULL)
        return;
    
    objects.at(index) = NULL;
    freelist.push_back(index);
}

// C API wrappers

struct sre_Object: sre::Object
{
    sre_Object(const sre_ObjectTable& ft): funcs([](const sre_ObjectTable& funcs) {
        return sre_ObjectTable{
            funcs.destroy ? funcs.destroy : [](void*) {},
            funcs.update ? funcs.update : [](void*) {},
            funcs.render ? funcs.render : [](void*) {}
        };
    }(ft)) {}
    const sre_ObjectTable funcs;

    ~sre_Object() override { funcs.destroy(this+1); }
    void update() override { funcs.update(this+1); }
    void render() override { funcs.render(this+1); }
};

sre_Object* sre_create_object(const sre_ObjectTable* functiontable, size_t extrasize)
{
    extrasize = ((extrasize + (alignof(sre_Object)-1))/alignof(sre_Object)) * alignof(sre_Object);
    void* ptr = operator new(sizeof(sre_Object) + extrasize);
    return new(ptr) sre_Object(*functiontable);
}

void sre_destroy_object(sre_Object* object)
{
    if (!object)
        return;
    object->destroy();
}

int sre_bind_objectlayer(sre_Object* object) {
    if (!object)
        return -1;
    
    return sre::bind_object_layer(*object);
}

void sre_unbind_objectlayer(int index) {
    sre::unbind_object_layer(index);
}

//