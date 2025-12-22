#ifndef SREECS_SCENE_HPP
#define SREECS_SCENE_HPP

#include <ECS/common.hpp>

#include <datatypes/color.hpp>
#include <datatypes/vector.hpp>

#include <Game/Camera.hpp>

#include <internal_def.hh>

__def_internal(__update_world);

namespace sreECS
{
    class Entity;
    struct Camera;

    struct Scene: public Common
    {
        __friend_internal(__update_world);
        friend class Entity;

        Scene();
        ~Scene();

    public:
        /// The background color that the game will have when there's a world, the alpha component is not used
        sre::col4 background{255, 149, 236};
        /// The color that the game will render on top of the world.
        /// This also acts as the background of the GUI Layer
        sre::col4 foreground{sre::col4::INVISIBLE};
        // The camera, whence its coordinates point to the center of the screen
        Game::Camera camera;
    public:
        // Make the scene current, this is the equivalent of calling: `sreECS::Scene::make_current(this, destroy_old)`
        void make_current(bool destroy_old=true) { return make_current(this, destroy_old); }
        // Make a scene current
        // @param scene the scene to be current. Can be `nullptr` if you want no scene to be current
        // @param destroy_old destroy and free, if there is, the old scene. Note that this is by default `true` in the non-static `make_current()` method
        static void make_current(Scene* scene, bool destroy_old);

        // Get the current scene, with a template in which `T` is a type inherited by Scene
        // @returns a pointer to a current scene, or `nullptr` if there's no current scene, or if the current scene isn't or doesn't inherit from `T`
        template <typename T=Scene> static inline T* current() { return current()->cast<T>(); /* Scary ! Note that if current() is nullptr then dynamic_cast will just return nullptr */ }

        // Get the current scene, without templates
        static Scene* current();

        // Calls dynamic_cast to it. It's also fine to just use dynamic_cast
        template <typename T> inline T* cast() const { return dynamic_cast<T*>(this); }

        // Entities

        template <typename T = Entity, typename... Args> T& add_entity(Args&&... args)
        {
            static_assert(std::is_base_of<Entity, T>::value, "T must be derived from Game::Entity");

            T* entity = alloc_entity(sizeof(T), NULL);
            entity->T(std::forward(args)...);

            return *entity;
        }

        // Iterating

        struct Iterator
        {
            constexpr Iterator(const size_t& ptr, const Scene* _this): m_ptr(&ptr), m_scene(_this)
            {
            }

            inline Entity& operator *() const { return *m_scene->entity_at(*m_ptr); }

            inline void operator ++()
            {
                m_ptr++;
            }
            inline Iterator operator ++(int)
            {
                Iterator tmp = *this;
                this->operator++();
                return tmp;
            }

            inline void operator --()
            {
                m_ptr--;
            }
            inline Iterator operator --(int)
            {
                Iterator tmp = *this;
                this->operator--();
                return tmp;
            }
            inline bool operator ==(const Iterator& other) const
            {
                return m_ptr == other.m_ptr;
            }
            inline bool operator !=(const Iterator& other) const
            {
                return m_ptr != other.m_ptr;
            }
        private:
            const size_t* m_ptr;
            const Scene* m_scene;
        };

        Iterator begin() const { return {*m_entities.begin(), this}; }
        Iterator end() const { return {*m_entities.end(), this}; }

    private:
        struct _Arena
        {
            static constexpr size_t PAGE_SIZE = 4096;
            static constexpr size_t SIZE = PAGE_SIZE - sizeof(_Arena*);

            _Arena* next;
            sre::byte data[SIZE];
        };
        struct _FreeList
        {
            size_t offset;
            size_t size;
        };
        _Arena* m_arenabuff;
        std::vector<size_t> m_entities;
        std::vector<_FreeList> m_freelist;

        Entity* alloc_entity(size_t size, size_t* realsize);
        Entity* entity_at(size_t offset) const;

        static _Arena* new_arena();
    private:
        void call_update();
    };
}

#endif
