#ifndef SREECS_SCENE_HPP
#define SREECS_SCENE_HPP

#include <Core/Object.hpp>
#include <Base/Signal.hpp>

#include <Datatypes/Color.hpp>
#include <Datatypes/Vector.hpp>

#include <ECS/Camera.hpp>

namespace sre
{
    class ECS;
    struct RenderInterface;
}

namespace sreECS
{
    class Entity;

    struct Scene: public sre::Object
    {
        friend class sre::ECS;
        friend class Entity;

        Scene();
        ~Scene();

        static const Entity* s_querying;
    public:
        /// The background color that the game will have when there's a world, the alpha component is not used
        sre::col4 background{255, 149, 236};
        /// The color that the game will render on top of the world.
        /// This also acts as the background of the GUI Layer
        sre::col4 foreground{sre::INVISIBLE};
        // The camera, whence its coordinates point to the center of the screen
        Camera camera;
    public:
        // Make the scene current, this is the equivalent of calling: `sreECS::Scene::make_current(this, destroy_old)`
        void make_current(bool destroy_old=true) { return make_current(this, destroy_old); }
        // Make a scene current
        // @param scene the scene to be current. Can be `nullptr` if you want no scene to be current
        // @param destroy_old destroy and free, if there is, the old scene. Note that this is by default `true` in the non-static `make_current()` method
        static void make_current(Scene* scene, bool destroy_old);

        // Get the current scene, with a template in which `T` is a type inherited by Scene
        // @returns a pointer to a current scene, or `nullptr` if there's no current scene, or if the current scene isn't or doesn't inherit from `T`
        template <typename T> static inline T* current() { return dynamic_cast<T*>(current()); }

        // Get the current scene, as the default `Scene` type
        static Scene* current();

        // Entities

        template <typename T = Entity, typename... Args> T& add_entity(Args&&... args)
        {
            static_assert(std::is_base_of<Entity, T>::value, "T must be derived from sreECS::Entity");

            Entity* entity = alloc_entity(sizeof(T), NULL);
            return *::new(static_cast<void*>(entity)) T(std::forward<Args>(args)...);
        }

        // Iterating

        struct Iterator
        {
            using iterator_category = std::random_access_iterator_tag;
            using value_type = Entity;
            using difference_type = std::ptrdiff_t;
            using pointer = value_type*;
            using reference = value_type&;

            Iterator() = default;
            Iterator(Entity* const& ptr): m_ptr(&ptr)
            {
            }

            inline reference operator *() const { return **m_ptr; }

            inline Iterator& operator ++()
            {
                m_ptr++;
                return *this;
            }
            inline Iterator operator ++(int)
            {
                Iterator tmp = *this;
                this->operator++();
                return tmp;
            }

            inline Iterator& operator --()
            {
                m_ptr--;
                return *this;
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
            Entity* const* m_ptr = NULL;
        };
        using ReverseIterator = std::reverse_iterator<Iterator>;


        Iterator begin() const {
            return {*m_entities.begin()};
        }
        Iterator end() const {
            return {*(&m_entities.back() + 1)};
        }

        ReverseIterator rbegin() const {
            return ReverseIterator(end());
        }
        ReverseIterator rend() const {
            return ReverseIterator(begin());
        }

    private:
        struct _Arena
        {
            #ifdef PAGE_SIZE
                #undef PAGE_SIZE
            #endif
            static constexpr size_t PAGE_SIZE = 4096;
            static constexpr size_t SIZE = PAGE_SIZE - sizeof(size_t);

            _Arena* next;
            sre::byte data[SIZE];
        };

        _Arena* m_arenabuff;
        Entity* m_entity_end;
        std::vector<Entity*> m_entities;
        std::vector<Entity*> m_freelist;

        Entity* alloc_entity(size_t size, size_t* realsize);

        static _Arena* new_arena();
    protected:
        virtual void update() {}
        virtual void pupdate() {}
        virtual void pre_render() {}
        virtual void post_render() {}
    public:
        sre::Signal<sre::empty_t> updated{this};
        sre::Signal<sre::empty_t> rendered{this};
    private:
        void call_update();
        void call_render();
        bool call_query(sre::vec2ut screen_coords) const;
    };

    using World = Scene; // Alias for `Scene`, for now. Might be moving back to World

    extern sre::vec2ut mouse_worldcoords(); // Helper to determine mouse world coordinates
}

#endif
