#include <Engine.h>

Audio test1, test2;

void Initialize()
{
    printf("Hello world!\n");

    AudioData& test1_data = Engine->AudioDevice.LoadAudio("res://test1.wav");
    AudioData& test2_data = Engine->AudioDevice.LoadAudio("res://test2.ogg");

    test1.Attach(test1_data);
    test2.Attach(test2_data);

    Engine->AudioDevice.PlayAudio(test2);
}