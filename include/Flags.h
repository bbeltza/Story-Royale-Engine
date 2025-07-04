#pragma once

enum FLAG_ShapeFlags : char
{
    VISIBLE = (1 << 1),
    CAN_TOUCH = (1 << 2),
    CAN_COLLIDE = (1 << 3),
    CAN_QUERY = (1 << 4),
};

enum FLAG_GuiComponentUpdateFlags : unsigned char
{
    NONE = 0,
    PROCESS_SIZE = (1 << 1),
    PROCESS_POSITION = (1 << 2),
    PROCESS_CHILDREN = (1 << 3),
    RENDER = (1 << 4),
    PRE_RENDER = (1 << 5),

    CUSTOM = NONE
};