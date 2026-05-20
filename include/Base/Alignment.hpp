#ifndef SRE_ALIGNMENT_HPP
#define SRE_ALIGNMENT_HPP

namespace sre
{
    enum alignment: char
    {
        ALIGN_LEFT,
        ALIGN_CENTER,
        ALIGN_RIGHT,

        ALIGN_TOP = ALIGN_LEFT,
        ALIGN_BOTTOM = ALIGN_RIGHT
    };
}

#endif