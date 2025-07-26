#include "Datatypes/UDim.h"

// UDim constructors

UDim::UDim():
    Scale(1),
    Offset(0)
{}

UDim::UDim(const float Scale, const int Offset) :
    Scale(Scale),
    Offset(Offset)
{}

UDim::UDim(const UDim& other) :
    Scale(other.Scale),
    Offset(other.Offset)
{}

// UDim2 constructors

UDim2::UDim2() :
    X(),
    Y()
{}

UDim2::UDim2(
    const float xScale, const int xOffset,
    const float yScale, const int yOffset
) :
    X(xScale, xOffset),
    Y(yScale, yOffset)
{}

UDim2::UDim2(const UDim& X, const UDim& Y) :
    X(X),
    Y(Y)
{}

UDim2::UDim2(const UDim2& other) :
    X(other.X),
    Y(other.Y)
{}

const UDim UDIM_ZERO = UDim(0, 0);
const UDim2 UDIM2_ZERO = UDim2(0, 0, 0, 0);