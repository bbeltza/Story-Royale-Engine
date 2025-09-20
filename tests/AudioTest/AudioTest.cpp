#include <Engine.h>

Audio stereotest;
Audio test1, test2, test3;

void Initialize()
{
    AudioData& stereotest_data = Engine->AudioDevice.LoadAudio("res://stereotest.wav");
    AudioData& test1_data = Engine->AudioDevice.LoadAudio("res://test1.wav");
    AudioData& test2_data = Engine->AudioDevice.LoadAudio("res://test2.ogg");
    AudioData& test3_data = Engine->AudioDevice.LoadAudio("res://test3.ogg");

    stereotest.Attach(stereotest_data);
    test1.Attach(test1_data);
    test2.Attach(test2_data);
    test3.Attach(test3_data);

    test3.Info.fade_in = 1;
    test3.Info.looped = true;
    test3.Info.volume = 2;
    test3.Info.speed = 1;

    stereotest.Info.looped = true;

    Engine->AudioDevice.PlayAudio(test3);
}