#include <GameSettings.h>

void GameSettings::onSet()
{
    GameSettings::StartResolution.width = 480 / 9 * 16;
    GameSettings::Title = "My Engine!";
    GameSettings::targetFPS = 0;
}