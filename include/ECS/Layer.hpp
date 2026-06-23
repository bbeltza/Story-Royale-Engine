#ifndef SREECS_LAYER_H
#define SREECS_LAYER_H
#include <ECS/Scene.hpp>
#include <SDL_mutex.h>

namespace sreECS
{
    class Layer;
    Layer* get_default_layer();

    struct Scene;
    class Layer: public sre::Object
    {
        friend struct Scene;

        Scene* m_current = NULL;
        sre::timeStamp m_pupdate_dt = 1 / 128.0_ts;
        sre::timeStamp m_pupdate_accum = 0.0_ts;
    public: 
        Layer();
        ~Layer();
        void update() override;
        void render() override;

        sre::timeStamp get_physics_dt() { return m_pupdate_dt; }
        void set_physics_dt(sre::timeStamp dt) {
            dt = dt < 0 ? -dt : dt; // `dt` Cannot be lower than zero, right here, invert the value if that's the case. Maybe return failure soon for later
                                        // `dt` can be 0, if so, then do not do any timestamp handling and pass pupdate with the ACTUAL game's delta-time

            m_pupdate_dt = dt;
        }

        Scene* get_current() { return m_current; }
        void set_current(Scene* scene);
    };
}


#endif