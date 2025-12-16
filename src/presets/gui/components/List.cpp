#include "Game/GuiObject.hpp"
#include "Game/GuiComponents/List.hpp"

void GuiComponents::List::process_children(Game::GuiObject* child, uint32_t index)
{
    if (!index)
        m_lastsize = 0;
    
    sre::rect2Dut* childabs = getAbsolute(child); 
    sre::rect2Dut* parentabs = getAbsolute(child->getParent()); 

    switch (direction)
    {
    case HORIZONTAL:
        childabs->position.y = parentabs->position.y;
        childabs->position.x = parentabs->position.x + m_lastsize;
        m_lastsize += childabs->size.x + Padding.to_absolute(parentabs->size.x);
        break;
    default:
        childabs->position.x = parentabs->position.x;
        childabs->position.y = parentabs->position.y + m_lastsize;
        m_lastsize += childabs->size.y + Padding.to_absolute(parentabs->size.y);
        break;
    }
    
}