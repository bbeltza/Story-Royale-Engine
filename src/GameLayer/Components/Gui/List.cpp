#include "Game/GuiObject.h"
#include "Game/GuiComponents/List.h"

void GuiComponents::List::process_children(Game::GuiObject* child, uint32_t index)
{
    if (!index)
        m_lastsize = 0;
    
    RectF* childabs = getAbsolute(child); 
    RectF* parentabs = getAbsolute(child->getParent()); 

    switch (direction)
    {
    case dir_horizontal:
        childabs->Position.Y = parentabs->Position.Y;
        childabs->Position.X = parentabs->Position.X + m_lastsize;
        m_lastsize += childabs->Size.X + Padding.toAbsolute(parentabs->Size.X);
        break;
    default:
        childabs->Position.X = parentabs->Position.X;
        childabs->Position.Y = parentabs->Position.Y + m_lastsize;
        m_lastsize += childabs->Size.Y+ Padding.toAbsolute(parentabs->Size.Y);
        break;
    }
    
}