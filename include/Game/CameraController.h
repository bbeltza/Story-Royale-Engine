#pragma once
#include "Datatypes/TimeStamp.h"

namespace Game
{
    struct Camera;

    class CameraController
    {
        friend struct Camera;
        
        public:
        CameraController();
        virtual ~CameraController();
        inline Camera* getCamera() const {return m_Camera;}

        bool Enabled = true;
        protected:
        virtual void Update(TimeStamp) = 0;
        private:
        Camera* m_Camera;
    };
}