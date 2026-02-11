#ifndef SREECS_ENTITY_HPP
#define SREECS_ENTITY_HPP

#include <ECS/common.hpp>
#include <Datatypes/Vector.hpp>

namespace sreECS
{
    struct Scene;
    struct Component;

    class Entity: public Common
    {
        friend struct Scene;

        //size_t m_offset;
        // Total allocated size in the Scene's arena
        size_t m_size;
        // Current scene parent
        Scene* m_parent;

    protected:
        Entity(): Entity(0, 0) {}
        Entity(sre::unit x, sre::unit y, long z_index=0);
        virtual ~Entity();
        void operator delete(void*);

        Entity(const Entity& copy) = delete;
        Entity(Entity&& moving); // For world reparenting, might start using it I don't know..
    public:
        // The current position of the entity
        sre::vec2ut position;
        // The position difference between the last and current frame,
        // useful for checking if an entity has been moved
        const sre::vec2ut lastVelocity;

        long z_index;

        // Gets the parent scene of the entity, templated by a derived `T` type from `Scene`
        // 
        // Note that `T` has to derive from `Scene`, otherwise a compiler error is expected
        // Note also that <ECS/scene.hpp> has to be included before using it
        // @return A pointer to the parent to the entity, or `nullptr` if the parent does not inherit `T`
        template <class T=Scene>
        T* get_parent() const { return dynamic_cast<T*>(m_parent); }
    public:
        // Attaches components to the entity
        // 
        // There's also a version that accepts Component pointers:
        // look into `setup_components(Component* first, Args...)`
        // 
        // Note also that if there were any existing components then they will be deattached from the entity
        // meaning this operation resets the entities too, and may reallocate more memory. So it's best practice to only use it once setting up your entity
        // 
        // @param first - the first component of the arguments, this is done to allow easier overloading and to make sure you don't call the function empty
        // @param rest - the rest of the components, note that they all have to be Component references, otherwise a compiler error is expected
        template <typename... Args>
        void setup_components(Component& first, Args&&... rest)
        {
            Component* arr[] = { &first, &rest... };
            return setup_components(arr, sizeof...(rest) + 1);
        }
        // Same as setup_components(Component& first, Args...), but accepts pointers instead of references
        template <typename... Args>
        void setup_components(Component* first, Args... rest)
        {
            Component* const arr[] = { first, rest... };
            return setup_components(arr, sizeof...(rest) + 1);
        }
        /*
          Sets up the entity's components, the C-style way.

          Note that this operation allocates memory, copying the `components` parameter, even if there are existing components,
          it only uses the existing allocated memory if the amount of components in the entity is the same as the amount of components passed in.

          This operation just attaches components, it allows the engine to know which components the entity has,
          it does not clean up the components once the entity gets destroyed
          @brief It is best practice to store all of your components inside your derived `Entity` class in order for them to be cleaned up,
          but they can be stored anywhere. You can have multiple entities point to the same component.

          This method can be used if you're more familiar with C-style array passing functions, or if you don't know what components to setup.
          Otherwise, look into `setup_components(Component& first, Args&&... rest)`, it might be more convinient.

          @param components - an array of component pointers
          @param count - the amount of pointers in `components`
        */
        void setup_components(Component* const components[], size_t count);

        // Iterators
        struct Iterator
        {
            Iterator(Component* const* ptr): m_ptr(ptr) {}

            Component& operator *() const { assert(m_ptr); return **m_ptr; }

            void operator ++() { ++m_ptr; }
            void operator --() { --m_ptr; }

            Iterator operator ++(int) { return m_ptr++; }
            Iterator operator --(int) { return m_ptr--; }

            bool operator !=(const Iterator& other) const { return m_ptr != other.m_ptr; }
        private:
            Component* const* m_ptr = NULL;
        };

        Iterator begin() const { return m_components; }
        Iterator end() const { return m_components + m_componentcount; }
        bool empty() const { return !m_componentcount; }

        size_t component_size() const { return m_componentcount; }

    private:
        Component** m_components = NULL;
        size_t m_componentcount = 0;
    };
}

#endif