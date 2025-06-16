#pragma once

enum FLAG_ShapeFlags : char
{
    VISIBLE = (1 << 0),
    CAN_TOUCH = (1 << 1),
    CAN_COLLIDE = (1 << 2),
};