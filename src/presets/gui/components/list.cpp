#include <GUI/Components/List.hpp>
#include <cstdlib>

using namespace sreGUI;

static sre::unit calc_widthoffset(sre::unit parent_l, sre::unit child_l, sre::alignment alignment)
{
    switch (alignment)
    {
    case sre::ALIGN_LEFT:
        return 0;
    case sre::ALIGN_RIGHT:
        return parent_l - child_l;
    case sre::ALIGN_CENTER:
        return (parent_l - child_l)/2;
    default:
        return 0;
    }
}

void List::process_children(const sre::rect2Dut& parent, sre::rect2Dut children[], size_t count)
{
    sre::unit offs = 0;
    sre::unit length = direction == HORIZONTAL ? parent.size.x : parent.size.y;
    sre::unit abspadding = padding.to_absolute(length);

    for (size_t i = 0; i < count; i++)
    {
        switch (direction)
        {
            case HORIZONTAL:
                children[i].position.y = parent.position.y + calc_widthoffset(parent.size.y, children[i].size.y, v_alignment);
                children[i].position.x = parent.position.x + offs;
                offs += children[i].size.x + abspadding;

                break;
            case VERTICAL:
                children[i].position.x = parent.position.x + calc_widthoffset(parent.size.x, children[i].size.x, h_alignment);
                children[i].position.y = parent.position.y + offs;
                offs += children[i].size.y + abspadding;
                
                break;
            default:
                return;
        }
    }

    sre::alignment content_alignment = direction == HORIZONTAL ? h_alignment : v_alignment;

    offs -= abspadding;
    switch (content_alignment)
    {
        case sre::ALIGN_LEFT: return;
        case sre::ALIGN_CENTER:
            offs = length/2 - offs/2;
            break;
        case sre::ALIGN_RIGHT:
            offs = length - offs;
            break;
        default:
            abort();
    }

    for (size_t i = 0; i < count; i++)
    {
        switch (direction)
        {
            case HORIZONTAL:
                children[i].position.x += offs;
                break;
            case VERTICAL:
                children[i].position.y += offs;
                break;
            default:
                return;
        }
    }
}