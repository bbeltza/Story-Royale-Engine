#include <Engine.h>

Audio test1;

void Initialize()
{
    printf("Hello world!\n");

    AudioData& test1_data = Engine->AudioDevice.LoadAudio("res://test1.wav");

    test1.Load(test1_data);

    Engine->AudioDevice.PlayAudio(test1);
}