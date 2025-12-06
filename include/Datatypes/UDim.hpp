#pragma once
#include <Datatypes/Vector.hpp>

struct UDim
{
    constexpr UDim(): Scale(0), Offset(100) {}
    constexpr UDim(float scale, int offset): Scale(scale), Offset(offset) {}
    constexpr UDim(const UDim& other): Scale(other.Scale), Offset(other.Offset) {}

    inline constexpr float toAbsolute(int relative) const { return relative * Scale + Offset; };
    inline constexpr float toAbsolute(float relative) const { return relative * Scale + Offset; };

    float Scale;
    int Offset;

    static const UDim ZERO;
};

struct UDim2
{
    constexpr UDim2() = default;
    constexpr UDim2(float xScale, int xOffset, float yScale, int yOffset): X(xScale, xOffset), Y(yScale, yOffset) {}
    constexpr UDim2(const UDim& X, const UDim& Y): X(X), Y(Y) {}
    constexpr UDim2(const UDim2& other): X(other.X), Y(other.Y) {}

    template <typename T1, typename T2, typename T3, typename T4>
    constexpr UDim2(T1 xScale, T2 xOffset, T3 yScale, T4 yOffset): UDim2(
        static_cast<float>(xScale),
        static_cast<int>(xOffset),
        static_cast<float>(yScale),
        static_cast<int>(yOffset)
    ) {}

    UDim X;
    UDim Y;

    static UDim2 FromOffset(const Vector2i& vec) { return { 0, vec.X, 0, vec.Y }; }

    static const UDim2 ZERO;
};