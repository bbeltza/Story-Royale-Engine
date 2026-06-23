#ifndef SRE_RECT_H
#define SRE_RECT_H

#include <Datatypes/Vector.h>

#ifdef __cplusplus

namespace sre
{
    template <typename T> using is_valid_rect_t = is_valid_vector_t<T>;

    template <typename T>
    union rect2D
    {
        using type = T;
        using vec = vec2<type>;

        struct {
            vec position;
            vec size;
        };
        struct {
            T x;
            T y;
            T w;
            T h;
        };

        constexpr rect2D(): position(), size() {}
        constexpr rect2D(T x, T y, T w, T h): position(x, y), size(w, h) {}
        constexpr rect2D(const vec& position, const vec& size): position(position), size(size) {}
        constexpr rect2D(const rect2D& copy): position(copy.position), size(copy.size) {}

        template <typename T2>
        explicit constexpr rect2D(const rect2D<T2>& other): position(other.position), size(other.size) {}

        template <typename T2>
        explicit constexpr operator rect2D<T2>() const {
            return rect2D<T2>{
                static_cast<typename rect2D<T2>::vec>(position),
                static_cast<typename rect2D<T2>::vec>(size)
            };
        }

        constexpr bool operator ==(const rect2D& other) const { return position == other.position && size == other.size; }
        constexpr bool operator !=(const rect2D& other) const { return position != other.position || size != other.size; }

        constexpr vec origin(sre::vec2ut uv) const { return vec{ position + size*uv }; }
        constexpr vec origin(sre::unit uv_x, sre::unit uv_y) const { return origin(sre::vec2ut{ uv_x, uv_y }); }

        constexpr rect2D origin_rect(sre::vec2ut uv) const { return rect2D{origin(uv), size}; }

        constexpr const T* ptr() const { return &x; } 
        inline          T* ptr()       { return &x; } 

        constexpr bool intersects(vec pt) const {
            return ( x < pt.x && y < pt.y ) &&
                    ( x+w > pt.x && y+h > pt.y );
        }
        constexpr bool intersects(const rect2D& other) const {
            return (x < other.x+other.w && y < other.y+other.h) &&
                    (x+w > other.x && y+h > other.y);
        }

        // Use these methods if your rect's origin is supposed to be in a place other than its top-left corner
        constexpr bool intersects_from_origin(vec pt, sre::vec2ut uv) const {
            return origin_rect(uv).intersects(pt);
        }
        constexpr bool intersects_from_origin(const rect2D& other, sre::vec2ut uv) const {
            return origin_rect(uv).intersects(other.origin_rect(uv));
        }
    };

    // A rect2D datatype composed of ints.
    using rect2Di = rect2D<int>;
    using rect2Du = rect2D<unsigned>;
    // A rect2D datatype composed of floats.
    using rect2Df = rect2D<float>;
    using rect2Dd = rect2D<double>;

    // A rect2D datatype composed of units ( floats for now )
    using rect2Dut = rect2D<unit>;

    template <typename T>
    constexpr rect2D<T> from_extents(vec2<T> begin, vec2<T> end) {
        return rect2D<T>{
            begin,
            end - begin
        };
    }
}

template <typename Char, typename Traits, typename T>
std::basic_ostream<Char, Traits>& operator <<(std::basic_ostream<Char, Traits>& os, const sre::rect2D<T>& rect)
{
    os << "( " << (rect.position.x + 0) << " ; " << (rect.position.y + 0) << " | " << (rect.size.x + 0) << " ; " << (rect.size.y) << " )";
    return os;
}

#define SRE_RECT2DMAKE(type, n) using n = ::sre::rect2D<type>
#define SRE_RECT2DMAKESFFX(type, n) using sre_rect2D##n = ::sre::rect2D<type>; using sre_vec2##n = ::sre::vec2<type>

#else
// C compatible rect 2D class templates + union

#define SRE_RECT2DMAKE(type, n)    \
typedef struct n { \
    type x; type y; type w; type h; \
} n

#define SRE_RECT2DMAKESFFX(type, n) SRE_VEC2MAKESFFX(type, n); \
typedef union sre_rect2D##n { \
    struct {type x; type y; type w; type h;}; \
    struct {sre_vec2##n position; sre_vec2##n size;}; \
} sre_rect2D##n


#endif

#endif