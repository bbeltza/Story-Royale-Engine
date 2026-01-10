#include <Datatypes/Color.hpp>
#include <Datatypes/UDim.hpp>

using namespace sre;

const col3 col3::WHITE = { 0xFF, 0xFF, 0xFF };
const col3 col3::BLACK = { 0x00, 0x00, 0x00 };
const col3 col3::RED = { 0xFF, 0x00, 0x00 };
const col3 col3::GREEN = { 0x00, 0xFF, 0x00 };
const col3 col3::BLUE = { 0x00, 0x00, 0xFF };

const col4 col4::WHITE = col3::WHITE;
const col4 col4::BLACK = col3::BLACK;
const col4 col4::INVISIBLE = { col3::BLACK, 0x00 };
const col4 col4::RED = col3::RED;
const col4 col4::GREEN = col3::GREEN;
const col4 col4::BLUE = col3::BLUE;

const udim udim::ZERO = { 0, 0 };
const udim udim::FULL = { 1, 0 };

const udim2 udim2::ZERO = { udim::ZERO, udim::ZERO };
const udim2 udim2::FULL = { udim::FULL, udim::FULL };