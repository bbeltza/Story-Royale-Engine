#pragma once
#include "Datatypes/TimeStamp.h"

namespace Game
{
    class CameraController
    {
        friend class Camera;
        public:

        private:
        void Update(TimeStamp);
    };
}