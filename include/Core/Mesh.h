#ifndef SRE_MESH_H
#define SRE_MESH_H
#include <C_API.h>
#include <ints.h>

SRE_CAPI_BEGIN

typedef sre_u32 sre_Mesh;

#include <Core/Draw.h>
typedef struct sre_DDmesh
{
	sre_s32 flags;
	union
	{
		sre_col4 color;
		sre_col4 modulate;
	};
	sre_vec2ut position;
	sre_vec2ut scale;
	sre_vec2ut anchor;
} sre_DDmesh;

enum sre_meshDimensions
{
	SRE_MESH_1D = 1,
	SRE_MESH_2D = 2,
	SRE_MESH_3D = 3,
	SRE_MESH_4D = 4
};

sre_Mesh sre_meshcreate();
bool sre_meshupdate(sre_Mesh mesh, sre_meshDimensions dimensions, const float* vertices, const sre_u8* indices, sre_u8 vertice_count, size_t indice_count);
void sre_meshdelete(sre_Mesh mesh);

bool sre_meshrender(sre_Mesh mesh, );

SRE_CAPI_END

#endif