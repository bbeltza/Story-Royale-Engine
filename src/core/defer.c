#include <Base/Defer.h>

#include "../internal.h"

struct sre_defer
{
    sre_deferFunction func;
    void* userdata;
    struct sre_defer* next;
};

void sre_defer(sre_deferFunction func, void* userdata)
{
    struct sre_defer* defer = malloc(sizeof(struct sre_defer));
    defer->func = func;
    defer->userdata = userdata;
    defer->next = engine.defer_head;

    engine.defer_head = defer;
}

void __call_deferred()
{
    while (engine.defer_head)
    {
        struct sre_defer* defer = engine.defer_head;
        defer->func(defer->userdata);

        engine.defer_head = defer->next;

        free(defer);
    }
}