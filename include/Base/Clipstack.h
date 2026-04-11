#ifndef SRE_CLIPSTACK_H
#define SRE_CLIPSTACK_H

#ifdef __cplusplus
    #include <Datatypes/Rect.hpp>
    #include <deque>

    namespace sre
    {
        // A container that sores and stacks rectangles inside of the last inserted rectangle
        // Note that the rectangles will be considered top-left cornered, this means its position represents the top-left corner of the rectangle (unlike in the rect2D methods)
        template <typename T, typename Cont=std::deque<sre::rect2D<T>>>
        class ClipStack
        {
            Cont m_stack;

        public:
            using Rect = sre::rect2D<T>;
            static constexpr T ZERO{0};

            bool pop()
            {
                m_stack.pop_back();
                return !m_stack.empty();
            }
            void push(Rect rectangle) {
                if (m_stack.empty())
                {
                    m_stack.push_back(rectangle);
                    return;
                }

                const Rect& toprect = m_stack.back();
                
                rectangle.position.x = toprect.position.x > rectangle.position.x ? toprect.position.x : rectangle.position.x;
                rectangle.position.x = toprect.position.y > rectangle.position.y ? toprect.position.y : rectangle.position.y;

                Rect::vec bounding = (toprect.position + toprect.size) - (rectangle.position + rectangle.size);
                rectangle.size.x = bounding.x < 0 ? rectangle.size.x + bounding.x : rectangle.size.x;
                rectangle.size.y = bounding.y < 0 ? rectangle.size.y + bounding.y : rectangle.size.y;

                m_stack.push_back(rectangle);
            }

            Rect top() const {
                if (m_stack.empty())
                    return {ZERO, ZERO, ZERO, ZERO};
                return m_stack.back();
            }
        };

        using ClipStackI = ClipStack<int>;
        using ClipStackUT = ClipStack<sre::unit>;
    }

#else
    #error Clipstack not implemented in C for now, it's going to be
#endif

#endif