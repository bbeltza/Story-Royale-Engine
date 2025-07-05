#pragma once

struct UDim
{
    UDim();
    UDim(const float Scale, const int Offset);
    UDim(const UDim& other);

    inline float toAbsolute(int relative) const { return relative * Scale + Offset; };

    float Scale;
    int Offset;
};

struct UDim2
{
    UDim2();
    UDim2(
        const float xScale, const int xOffset,
        const float yScale, const int yOffset
    );
    UDim2(const UDim& X, const UDim& Y);
    UDim2(const UDim2& other);

    UDim X;
    UDim Y;
};

extern const UDim UDIM_ZERO;
extern const UDim2 UDIM2_ZERO;