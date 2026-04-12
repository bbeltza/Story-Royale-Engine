#include "gl21.h"

bool sregl21_setup_texture(sregl21_inst* inst, sre_Sampler* texture, sre_pixelFormat format, int w, int h)
{
    return false;
}

bool sregl21_update_texture(sregl21_inst* inst, sre_Sampler* texture, const void* pixels, int pitch)
{
    return false;
}

bool sregl21_query_texture(sregl21_inst* inst, sre_Sampler* texture, int size[2], sre_pixelFormat* format)
{
    return false;
}

void sregl21_destroy_texture(sregl21_inst* inst, sre_Sampler* texture)
{

}