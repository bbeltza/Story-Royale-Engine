#include <Core/Audio.hpp>
#include <Entry.h>

sre::Audio stereotest;
sre::Audio test1, test2, test3;

void sre::initialize()
{
    stereotest.load("res://stereotest.wav");
    test1.load("res://test1.wav");
    test2.load("res://test2.ogg");
    test3.load("res://test3.ogg");

    test3.loop_point = 0;

    test3.fade_in(1);
}