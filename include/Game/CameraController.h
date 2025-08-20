#pragma once
#include "Datatypes/TimeStamp.h"

namespace Game
{
    class Camera;

    class CameraController
    {
        friend class Camera;
        
        public:
        CameraController();
        virtual ~CameraController();
        inline Camera* getCamera() const {return m_Camera;}
        protected:
        virtual void Update(TimeStamp) = 0;
        private:
        Camera* m_Camera;
    };
}