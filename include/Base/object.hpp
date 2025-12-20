#ifndef SRE_OBJECT_HPP
#define SRE_OBJECT_HPP

// C style object concept
/*
typedef struct
{
    void (*destroy)(sre_Object*);

    void (*update)(sre_Object*);
    void (*pupdate)(sre_Object*);
    void (*pre_render)(sre_Object*);
    void (*post_render)(sre_Object*);
} sre_Object;
*/

namespace sre
{
    struct Object
    {
        virtual void update() {}
        virtual void pupdate() {}

        virtual void pre_render() {}
        virtual void post_render() {}

        void destroy();

        private:
        Object() {}
        virtual ~Object();
    };
}

#endif