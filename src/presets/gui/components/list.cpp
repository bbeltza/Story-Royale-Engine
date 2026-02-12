#include <GUI/Components/List.hpp>

using namespace sreGUI;

void List::process_children(const sre::rect2Dut& parent, sre::rect2Dut children[], size_t count)
{
    sre::unit offs{};

    for (size_t i = 0; i < count; i++)
    {
        switch (direction)
        {
            case HORIZONTAL:
                children[i].position.y = parent.position.y;
                children[i].position.x = parent.position.x + offs;
                offs += children[i].size.x + padding.to_absolute(parent.size.x);
                break;
            case VERTICAL:
                children[i].position.x = parent.position.x;
                children[i].position.y = parent.position.y + offs;
                offs += children[i].size.y + padding.to_absolute(parent.size.y);
                break;
            default:
                return;
        }
    }    
}