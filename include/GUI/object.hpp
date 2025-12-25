#ifndef SREGUI_OBJECT_HPP
#define SREGUI_OBJECT_HPP

#include <list>

#include <datatypes/rect.hpp>

#include <Base/object.hpp>
#include <Base/Signal.hpp>

namespace sreGUI
{
    class Component;

    class Object : public ::sre::Object
    {
        Object *m_parent;

        sre::rect2Dut m_absolute = {0, 0, 0, 0};

    public:
        // Instantiating

        Object();
        ~Object();
        static void *operator new(std::size_t size)
        {
            auto ptr = static_cast<Object *>(::operator new(size));
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
            ::new (ptr) T(args...);
            return *ptr;
        }

    private:
        template <typename T>
        inline T *alloc_child()
        {
            static_assert(std::is_base_of<Object, T>::value, "T must be derived from sreGUI::Object");
            auto ptr = static_cast<T *>(::operator new(sizeof(T)));
            ptr->m_parent = this;
            return ptr;
        }

    public:
        // Parenting

        // Set the object to the root one, this is the equivalent of calling: set_parent(NULL)
        void set_root() { return set_parent(NULL); }
        void set_parent(Object *parent);

        inline Object *get_parent() const { return m_parent; }

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

        } children;
        class CContainer
        {
            friend class Object;
            Component *const *m_ptr = NULL;
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

    protected:
        virtual void update() {}
        virtual void pre_render() {}
        virtual void post_render() {}

    public:
        Signal<> updated{this};
        Signal<> rendered{this};
    };
}

#endif