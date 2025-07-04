#include "GameSettings.h"

const char* GameSettings::Title = "My Game";

GameSettings::t2 GameSettings::StartResolution = { 640, 480 };

uint16_t GameSettings::targetFPS = 60;
uint8_t GameSettings::windowFlags = 0;
uint8_t GameSettings::renderMode = 0;