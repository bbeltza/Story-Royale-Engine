#include <GUI/Components/List.hpp>
#include <cstdlib>

using namespace sreGUI;

void List::process_children(const sre::rect2Dut& parent, sre::rect2Dut children[], size_t count)
{
    sre::unit offs = 0;
    sre::unit absolute = direction == HORIZONTAL ? parent.size.x : parent.size.y;
    sre::unit abspadding = padding.to_absolute(absolute);

    for (size_t i = 0; i < count; i++)
    {
        switch (direction)
        {
            case HORIZONTAL:
                children[i].position.y = parent.position.y;
                children[i].position.x = parent.position.x + offs;
                offs += children[i].size.x + abspadding;
                break;
            case VERTICAL:
                children[i].position.x = parent.position.x;
                children[i].position.y = parent.position.y + offs;
                offs += children[i].size.y + abspadding;
                break;
            default:
                return;
        }
    }

    offs -= abspadding;
    switch (alignment)
    {
        case sre::A_LEFT: return;
        case sre::A_CENTER:
            offs = absolute/2 - offs/2;
            break;
        case sre::A_RIGHT:
            offs = absolute - offs;
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