#ifndef SRE_OBJECT_HPP
#define SRE_OBJECT_HPP

// C style object concept (Very susceptible to be changed or to just be thrown away)
typedef enum
{
    SRE_PIPELINE_UPDATE,
    SRE_PIPELINE_RENDER
} sre_PipelineType;

typedef struct sre_Object
{
    void (*destroy)(struct sre_Object* obj);
    void (*update)(struct sre_Object* obj, sre_PipelineType type);

    sre_Object *next;
} sre_Object;

#endif