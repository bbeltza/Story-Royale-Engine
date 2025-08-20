#pragma once
#include "Game/Camera.h"

#define use_namespace \
using namespace Game; \
using namespace CameraControllers;

#define cam_controldef(t) class t: public Game::CameraController
