#ifndef SREECS_SCENE_HPP
#define SREECS_SCENE_HPP

#include <Core/Object.h>
#include <Base/Signal.hpp>

#include <Datatypes/Color.h>
#include <Datatypes/Vector.h>

#include <ECS/Camera.hpp>

namespace sreECS
{
    class Layer;
    class Entity;
    struct Scene: public sre::Object
    {
        friend class Entity;
        friend class Layer;

        Scene() {}
        ~Scene();

        Layer* m_attachedlyr = NULL;
        const Entity* m_querying = NULL;
    public:
        /// The background color that the game will have when there's a world, the alpha component is not used
        sre::col4 background{255, 149, 236};
        /// The color that the game will render on top of the world.
        /// This also acts as the background of the GUI Layer
        sre::col4 foreground{sre::INVISIBLE};
        // The camera, whence its coordinates point to the center of the screen
        Camera camera;
    public:
        // Entities

        Entity* add_child(Entity* entity);

        template <typename T = Entity, typename... Args> T& add_entity(Args&&... args) {
            return static_cast<T&>(*add_child(new T(std::forward<Args>(args)...)));
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
        std::vector<Entity*> m_entities;
    protected:
        virtual void update() {}
        virtual void pupdate(sre::timeStamp) {}
        virtual void pre_render() {}
        virtual void post_render() {}
    public:
        sre::Signal<sre::empty_t> updated{this};
        sre::Signal<sre::empty_t> rendered{this};
    private:
        void call_update();
        void call_pupdate(sre::timeStamp dt);
        void call_render();
        bool call_query(sre::vec2ut screen_coords);

        sre::vec2ut process_camera();
        inline void clamp_camera() { camera.clamp_position(); }
    };

    //using World = Scene; // Alias for `Scene`, for now. Might be moving back to World

    extern sre::vec2ut mouse_worldcoords(); // Helper to determine mouse world coordinates

    void set_physics_dt(sre::timeStamp dt, Layer* lyr=NULL);
    sre::timeStamp get_physics_dt(Layer* lyr=NULL);
    
    void set_current(Scene* scene, Layer* lyr=NULL);
    Scene* get_current(Layer* lyr=NULL);
    // Get the current scene, with a template in which `T` is a type inherited by Scene
    // @param lyr The object layer that contains the scene. Must be NULL for now
    // @returns a pointer to a current scene, or `nullptr` if there's no current scene, or if the current scene isn't or doesn't inherit from `T`
    template <typename T> static inline T* get_current(Layer* lyr = NULL) { return dynamic_cast<T*>(get_current(lyr)); }
}

#endif
