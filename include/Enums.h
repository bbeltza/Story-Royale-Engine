#pragma once

enum ENUM_ParentType : unsigned char
{
    WORLD,
    COMPONENT
};
enum ENUM_ScaleType : unsigned char
{
    XY,
    XX
};
enum ENUM_ComponentType : unsigned char
{
    NULLCOMPONENT = 0,
    SHAPE,
    SPRITE,
    VELOCITY,
    ENTITY_CONTAINER
};

enum ENUM_Shape : char
{
    RECTANGLE,
    CIRCLE,
};

enum ENUM_FileType : unsigned char
{
    UNINITIALIZED = 0,
    TEXT,
    IMAGE,
    FONT,
    UNKNOWN
};