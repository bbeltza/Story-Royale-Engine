#ifndef SREECS_COMMON_HPP
#define SREECS_COMMON_HPP

namespace sreECS
{
    class Common
    {
        virtual void update();
        virtual void pupdate();
        virtual void pre_render();
        virtual void post_render();
    };
}

#endif