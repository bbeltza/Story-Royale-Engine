// Shaders used by d3d11 (They were the original shaders for d3d12 too)
// They are compiled and put into `d3d11shaders.cpp` as an array of bytes with the command line tool `fxc`

//      fxc /T ps_4_0 /Fh ps.h /E PSmain d3d11shaders.hlsl
//      fxc /T vs_4_0 /Fh vs1.h /E D1main d3d11shaders.hlsl
//      fxc /T vs_4_0 /Fh vs2.h /E D2main d3d11shaders.hlsl

// For d3d12's root signature: 
//      fxc /T rootsig_1_0 /E D3D12RootSig /Fh ../"direct3d12 (joke)"/rs.h d3d11shaders.hlsl

#define D3D12RootSig \
        "RootFlags ( ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |"     \
                    "DENY_HULL_SHADER_ROOT_ACCESS |"           \
                    "DENY_DOMAIN_SHADER_ROOT_ACCESS |"         \
                    "DENY_GEOMETRY_SHADER_ROOT_ACCESS),"       \
        "CBV(b0, visibility=SHADER_VISIBILITY_VERTEX),"           \
        "DescriptorTable( SRV(t0),"                               \
                        "visibility=SHADER_VISIBILITY_PIXEL ),"   \
        "RootConstants(num32BitConstants=2, b1, visibility=SHADER_VISIBILITY_VERTEX),"    \
        "StaticSampler( s0, filter=FILTER_MIN_MAG_MIP_POINT," \
                           "addressU=TEXTURE_ADDRESS_CLAMP," \
                           "addressV=TEXTURE_ADDRESS_CLAMP," \
                           "addressW=TEXTURE_ADDRESS_CLAMP," \
                           "borderColor=STATIC_BORDER_COLOR_TRANSPARENT_BLACK," \
                           "visibility=SHADER_VISIBILITY_PIXEL)"

struct D1input
{
    float4 transform: POSITION0;
    float2 anchor: POSITION1;
    float4 color: COLOR;
    float angle: PSIZE;

    float2 tuv: TEXCOORD0;
    float2 toffset: TEXCOORD1;
};

struct D2input
{
    float4 color: COLOR;
    float2 position: POSITION;
    float2 uv: TEXCOORD;
};

struct PSinput
{
    float4 vposition: SV_POSITION;
    float4 color: COLOR;

    float2 tuv: TEXCOORD0;
};

cbuffer CBuniforms: register(b0) {
    float4x4 VIEWPORT;
};

float2 CAMERA: register(b1);

PSinput D1main(D1input input, uint vid: SV_VertexID)
{
    static float4 VERTICES[] =
    {
        float4(0, 0, 0, 1),
        float4(1, 0, 0, 1),
        float4(0, 1, 0, 1),
        float4(1, 1, 0, 1)
    };

    float4x4 transform = float4x4(
        input.transform.z, 0, 0, 0,
        0, input.transform.w, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );

    float cx = cos(input.angle);
    float sx = sin(input.angle);
    float4x4 rotation = float4x4(
        cx, sx, 0, 0,
        -sx, cx, 0, 0,
        0, 0, 1, 0,
        input.transform.xy, 0, 1 
    );

    float4 vert = VERTICES[vid] - float4(input.anchor, 0.0, 0.0);
    vert = mul(vert, mul(transform, rotation));
    vert = ceil(vert * VIEWPORT[2][2] + float4(CAMERA, 0, 0));
    vert.w = 1;
    vert = mul(VIEWPORT, vert);

    PSinput output = {
        vert,
        input.color,
        VERTICES[vid].xy * input.tuv + input.toffset,
    };
    return output;
}

PSinput D2main(D2input input)
{
    float4 vert = float4(input.position, 0, 1);
    vert.xy = ceil(vert.xy * VIEWPORT[2][2]);
    vert.xy += CAMERA;
    vert = mul(VIEWPORT, vert);

    PSinput output = {
        vert,
        input.color,
        input.uv
    };
    return output;
}

SamplerState sstate: register(s0);
Texture2D tex: register(t0);

float4 PSmain(PSinput input): SV_TARGET
{
    return tex.Sample(sstate, input.tuv) * input.color;
}