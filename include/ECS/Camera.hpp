#ifndef SREECS_CAMERA_HPP
#define SREECS_CAMERA_HPP

#include <Datatypes/Rect.h>
#include <Datatypes/TimeStamp.h>

namespace sreECS
{
	struct Camera;

    struct CameraController
    {
        friend struct Camera;
        bool enabled = true;
    protected:
        virtual void on_update(Camera& camera) {}
        virtual void on_pupdate(Camera& camera, sre::timeStamp dt) = 0;
    };

    struct CameraEffect
    {
        friend struct Scene;
        bool enabled = true;
    protected:
        virtual sre::vec2ut on_process(sre::timeStamp dt) = 0;
    };

    struct Camera
    {
        friend struct Scene;

        sre::vec2ut position;
        sre::vec2ut scale; // Unused (for now)
        sre::rect2Dut bounds = { sre::vec2ut::ZERO, sre::vec2ut::ZERO };

        CameraController* controller = NULL;
        CameraEffect* effect = NULL; // For now there will only be support for a single effect, I might support multiple effects too
    public:
        // Convert world coordinates to screen coordinates
        // @param world_coords The world coordinates
        // @returns The screen coordinates
        sre::vec2ut toScreenSpace(sre::vec2ut world_coords) const;
        sre::vec2ut toWorldSpace(sre::vec2ut screen_coords) const;

        inline sre::vec2ut toScreenSpace(sre::unit x, sre::unit y) const { return toScreenSpace({x, y}); }
        inline sre::vec2ut toWorldSpace(sre::unit x, sre::unit y) const { return toWorldSpace({x, y}); }
    private:
        void update();
        void pupdate(sre::timeStamp dt);
        sre::vec2ut process();
        
        inline void clamp_position() {
            if (bounds.size != bounds.size.ZERO)
                position.setclamp(bounds.origin(-0.5f), bounds.origin(0.5f));
        }
    };
}

#endif
