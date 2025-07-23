#include "GuiComponents.h"

void Game::GuiComponents::UIList::process_children(GuiObject* child, uint32_t index)
{
    if (!index)
        m_lastsize = 0;
    
    switch (direction)
    {
    case dir_horizontal:
        getAbsolute(child)->y = getParentAbs()->y;
        getAbsolute(child)->x = getParentAbs()->x + m_lastsize;
        m_lastsize += getAbsolute(child)->w + Padding.toAbsolute(getParentAbs()->w);
        break;
    default:
        getAbsolute(child)->x = getParentAbs()->x;
        getAbsolute(child)->y = getParentAbs()->y + m_lastsize;
        m_lastsize += getAbsolute(child)->h + Padding.toAbsolute(getParentAbs()->h);
        break;
    }
    
}