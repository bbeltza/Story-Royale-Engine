#include "GuiComponents.h"

void Game::GuiComponents::UIModulate::pre_render()
{
    Color4* _mod = getParentMod();

    _mod->r *= Value.r / 255.0f;
    _mod->g *= Value.g / 255.0f;
    _mod->b *= Value.b / 255.0f;
    _mod->a *= Value.a / 255.0f;
}