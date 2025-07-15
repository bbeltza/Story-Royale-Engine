#include "GameSettings.h"

const char* GameSettings::Title = "My Game";

Vector2u GameSettings::StartResolution = { 640, 480 };
Vector2u GameSettings::ScalingResolution = {0, 0};

uint16_t GameSettings::targetFPS = 60;
uint8_t GameSettings::windowFlags = 0;
uint8_t GameSettings::renderMode = 0;