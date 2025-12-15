#include <datatypes/color.hpp>

using namespace sre;

const col3 col3::WHITE = { 0xFF, 0xFF, 0xFF };
const col3 col3::BLACK = { 0x00, 0x00, 0x00 };

const col4 col4::WHITE = col3::WHITE;
const col4 col4::BLACK = col3::BLACK;
const col4 col4::INVISIBLE = { col3::BLACK, 0x00 };