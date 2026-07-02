#ifndef SREGUI_OBJECT_HPP
#define SREGUI_OBJECT_HPP

#include <list>
#include <deque>

#include <Datatypes/Rect.h>
#include <Datatypes/Flags.hpp>

#include <Core/Object.h>
#include <Base/Signal.hpp>
#include <Base/Clipstack.h>

//#include <initializer_list>

namespace sreGUI
{
    struct Component;
    class Layer;

    using ComponentList = std::initializer_list<sreGUI::Component* const>;

    class Object : public ::sre::Object
    {
        friend class Layer;
        Layer* m_attachedlyr = NULL;
        Object* m_parent = NULL;

        sre::rect2Dut m_absolute = {0, 0, 0, 0};
        sre::flags16 m_state = {};
    public:
        enum State
        {
            S_INCURSOR = ut_bit(0),
            S_HOVERING = ut_bit(1),
            S_FOCUS = ut_bit(2) // Set to whether you're focusing the GUI area on the screen, will be working on that soon.
        };
        enum Flags
        {
            F_ENABLED = ut_bit(0),
            F_QUERY = ut_bit(1),
            F_CLIP = ut_bit(2)
        };

        // Flags that control the behavior of the object and its children
        sre::flags16 flags = {F_ENABLED, F_QUERY};
        int z_index = 0;
    public:
        // Instantiating

        Object(sreGUI::Object* parent=NULL, sreGUI::Component* const components[]=NULL, size_t num_components=0);
        ~Object();

        Object(sreGUI::Object* parent, sreGUI::ComponentList components): Object(parent, components.begin(), components.size()) {}
        Object(sreGUI::ComponentList components): Object(NULL, components) {}

        // This is a wrapper around `new` and `set_parent` for backwards compatibility with the old previous `add_child` function
        // To create a GUI object. use `new` and call `set_parent` on it to reparent it into another object. Eventually, you can also
            // use the parent constructor in the Object and pass the parent on it (which requires some setup on the derived class)
        template <typename T = Object, typename... Args>
        T &add_child(Args &&...args) {
            Object* pobj = new T(std::forward<Args>(args)...);
            return *static_cast<T*>(pobj->set_parent(this));
        }
    public:
        // Parenting
        Object* set_parent(Object *parent);

        template <typename T=Object>
        inline T *get_parent() const {
            return dynamic_cast<T*>(m_parent);
        }

        // Return `this` as the base sreGUI::Object* type
        inline Object* base() {
            return this;
        }
        inline const Object* base() const {
            return this;
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
        inline bool is_hovering(bool hastobeontop=true) const {
            if (hastobeontop)
                return m_state.has(S_HOVERING);
            return m_state.has(S_INCURSOR);
        }
        inline sre::flags16 get_state() const {
            return m_state;
        }
    protected:
        virtual void update() {}
        virtual void pre_render() {}
        virtual void render() {}
        virtual void post_render() {}
    private:
        void call_query(sre::vec2ut pt, std::deque<const Object*>& stack);
        void call_render(sre::ClipStackUT& clipstack);
        void call_update();

        void call_process();
        void call_processchildren();
        void call_prerender();
    public:
        sre::Signal<sre::empty_t> updated{this};
        sre::Signal<sre::empty_t> rendered{this};
    };

    // Layer functions

    const std::deque<const Object*>& get_hovering(Layer* lyr=NULL);

    void set_root(Object* object, Layer* lyr=NULL);
    Object* get_root(Layer* lyr=NULL);

    sre::unit get_insets(Layer* lyr=NULL);
    sre::unit get_viewport_scale(Layer* lyr=NULL); // This will return 0 if the scale is set to 0 (the default)! You may want to call sre::window_getscale() to get the real scale
    sre::rect2Dut get_viewport_area(Layer* lyr=NULL);

    void set_insets(sre::unit insets, Layer* lyr=NULL);
    void set_viewport(sre::rect2Dut area, sre::unit scale=0, Layer* lyr=NULL);
//
    inline bool is_hovering_anything(Layer* lyr=NULL) {
        return !get_hovering(lyr).empty();
    }
    template <typename T>
    inline T* get_root(Layer* lyr=NULL) { return dynamic_cast<T*>(get_root(lyr)); }
}

#endif