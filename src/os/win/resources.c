// This file doesn't depend of any os header, but it is still OS speficic

#include "logging.h"
#include <Windows.h>

extern const unsigned char* _game_res;
static const char nullbyte = '\0';

void SRE_init_resources()
{
    if (_game_res)
    {
        WARN("SRE_init_resources(): Attempting to initialize resources again, skipping...");
        return;
    }

    HRSRC res = FindResourceA(NULL, "GAME_RES", MAKEINTRESOURCE(RT_RCDATA));
    if (!res)
    {
        _game_res = &nullbyte;
        return;
    }
    HGLOBAL global = LoadResource(NULL, res);
    _game_res = LockResource(global);
}