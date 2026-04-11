#ifndef SREGUI_OBJECT_HPP
#define SREGUI_OBJECT_HPP

#include <list>

#include <Datatypes/Rect.hpp>
#include <Datatypes/Flags.hpp>

#include <Core/Object.hpp>
#include <Base/Signal.hpp>
#include <Base/Clipstack.h>

namespace sre
{
    struct RenderInterface;
    class ECS;
}

namespace sreGUI
{
    struct Component;

    class Object : public ::sre::Object
    {
        friend class sre::ECS;

        static sre::ClipStackUT s_clipstack;
        static const Object *s_querying;
        Object * volatile m_parent;
        //          ^^^   m_parent could be optimized away when `add_child` gets called and doesn't assign it

        sre::rect2Dut m_absolute = {0, 0, 0, 0};
    public:
        enum FlagsEnum
        {
            F_ENABLED = ut_bit(0),
            F_QUERY = ut_bit(1),
            F_CLIP = ut_bit(2)
        };
        // Flags that control the behavior of the object and its children
        sre::flags32 flags = {F_ENABLED, F_QUERY};
        int z_index = 0;
    public:
        // Instantiating

        Object();
        ~Object();
        static void *operator new(std::size_t size)
        {
            auto ptr = static_cast<Object *>(::operator new(size));
            #ifndef NDEBUG // Easier for catching lack of initialization bugs
                memset(static_cast<void*>(ptr), 0, size);
            #endif
            ptr->m_parent = NULL;

            return ptr;
        }

        template <typename T = Object>
        T &add_child()
        {
            auto ptr = alloc_child<T>();
            ::new (ptr) T; // If T has no constructor and calls T() then it'll be filled with zeros, we don't want that
            return *ptr;
        }

        template <typename T = Object, typename... Args>
        T &add_child(Args &&...args)
        {
            auto ptr = alloc_child<T>();
            ::new (ptr) T(std::forward<Args>(args)...);
            return *ptr;
        }

    private:
        template <typename T>
        inline T *alloc_child()
        {
            static_assert(std::is_base_of<Object, T>::value, "T must be derived from sreGUI::Object");
            auto ptr = static_cast<Object *>(::operator new(sizeof(T)));
            #ifndef NDEBUG
                memset(static_cast<void*>(ptr), 0, sizeof(T));
            #endif
            ptr->m_parent = this;
            return static_cast<T*>(ptr);
        }

    public:
        // Parenting

        // Set the object to the root one, this is the equivalent of calling: set_parent(NULL)
        void set_root(bool destroy_old=true);
        void set_parent(Object *parent);

        // Removes the current root object and destroys/cleans it if `destroy_old` is set to `true`
        static void remove_root(bool destroy_old=true);

        template <typename T=Object>
        inline T *get_parent() const
        {
            #ifdef assert
                assert(this != NULL && "Attempting to return parent of NULL object!");
            #endif
            return dynamic_cast<T*>(m_parent);
        }

    public:
        // Iterating

        class Container : std::list<Object *>
        {
            friend class Object;

        public:
            using list_type = std::list<Object *>;
            class Iterator
            {
            public:
                using iterator_category = std::bidirectional_iterator_tag;
                using value_type = Object;
                using difference_type = list_type::difference_type;
                using pointer = Object *;
                using reference = Object &;

                explicit Iterator(const list_type::const_iterator &it) : m_iterator(it) {}
                bool operator!=(const Iterator &other) const { return m_iterator != other.m_iterator; }
                Iterator &operator++()
                {
                    ++m_iterator;
                    return *this;
                }
                Iterator &operator--()
                {
                    --m_iterator;
                    return *this;
                }
                Iterator operator++(int)
                {
                    auto tmp = *this;
                    ++m_iterator;
                    return tmp;
                }
                Iterator operator--(int)
                {
                    auto tmp = *this;
                    --m_iterator;
                    return tmp;
                }
                reference operator*() const { return *(*m_iterator); }

            private:
                list_type::const_iterator m_iterator;
            };

            Iterator begin() const { return Iterator(list_type::begin()); }
            Iterator end() const { return Iterator(list_type::end()); }

            using list::size;
            using list::front;
            using list::back;
        } children;
        class CContainer
        {
            friend class Object;
            Component **m_ptr = NULL;
            size_t m_count = 0;

        public:
            // Pretty much like in sreECS::Entity

            template <typename... Args>
            void setup(Component &first, Args &&...rest)
            {
                Component *const arr[] = {&first, &rest...};
                return setup(arr, sizeof...(rest) + 1);
            }
            template <typename... Args>
            void setup(Component *first, Args... rest)
            {
                Component *const arr[] = {first, rest...};
                return setup(arr, sizeof...(rest) + 1);
            }

            void setup(Component *const components[], size_t count);
        public:
            class Iterator
            {
                Component *const *m_ptr;
            public:
                explicit Iterator(Component* const *ptr): m_ptr(ptr) {}
                bool operator !=(const Iterator& other) const { return m_ptr != other.m_ptr; }
                Component& operator*() const { return *(*m_ptr); }
                Iterator& operator++()
                {
                    m_ptr++;
                    return *this;
                }
                Iterator& operator--()
                {
                    m_ptr--;
                    return *this;
                }
                Iterator operator++(int)
                {
                    auto tmp = *this;
                    m_ptr++;
                    return tmp;
                }
                Iterator operator--(int)
                {
                    auto tmp = *this;
                    m_ptr--;
                    return tmp;
                }
            };

            Iterator begin() const { return Iterator(m_ptr); }
            Iterator end() const { return Iterator(m_ptr + m_count); }
        } components;
    public:
        bool is_hovering() const { return s_querying == this; }
        static bool hovering_anything() { return s_querying != NULL; /*This should be the equivalent of !((Object*)nullptr)->is_hovering()*/ }
    protected:
        virtual void update() {}
        virtual void pre_render(sre::RenderInterface* renderer) {}
        virtual void post_render(sre::RenderInterface* renderer) {}
    private:
        bool call_query(sre::vec2ut pt) const;
        void call_update();
        void call_render(sre::RenderInterface* renderer);

        void call_process();
        void call_processchildren();
        void call_prerender();

    public:
        sre::Signal<sre::empty_t> updated{this};
        sre::Signal<sre::empty_t> rendered{this};
    };

    Object* get_root();
    template <typename T>
    inline T* get_root() { return dynamic_cast<T*>(get_root()); }

    sre::unit get_insets();
    void set_insets(sre::unit insets); // Set left and right GUI insets. It is shared globally across the program
}

#endif