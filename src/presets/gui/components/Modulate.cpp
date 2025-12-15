#include "Game/GuiComponents/Modulate.hpp"

void GuiComponents::Modulate::pre_render(Game::GuiContainer* obj)
{
    if (*(uint32_t*)&Value == UINT32_MAX) return;

    sre::col4* _mod = getModulate(obj);

    float values[4] = {
        Value.r / 255.0f,
        Value.g / 255.0f,
        Value.b / 255.0f,
        Value.a / 255.0f,
    };

    _mod->r *= (uint8_t)values[0];
    _mod->g *= (uint8_t)values[1];
    _mod->b *= (uint8_t)values[2];
    _mod->a *= (uint8_t)values[3];
    
}