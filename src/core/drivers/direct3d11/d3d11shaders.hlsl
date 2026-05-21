// Shaders used by d3d11 (They were the original shaders for d3d12 too)
// They are compiled and put into `d3d11shaders.cpp` as an array of bytes with the command line tool `fxc`

//      fxc /T ps_4_0 /Fh ps.h /E PSmain d3d11shaders.hlsl
//      fxc /T vs_4_0 /Fh vs1.h /E D1main d3d11shaders.hlsl
//      fxc /T vs_4_0 /Fh vs2.h /E D2main d3d11shaders.hlsl

struct D1input
{
    float4 transform: POSITION0;
    float2 anchor: POSITION1;
    uint4 color: COLOR;
    float angle: PSIZE;

    float2 tuv: TEXCOORD0;
    float2 toffset: TEXCOORD1;
};

struct D2input
{
    uint4 color: COLOR;
    float2 position: POSITION;
    float2 uv: TEXCOORD;
};

struct PSinput
{
    float4 vposition: SV_POSITION;
    float4 color: COLOR;

    float2 tuv: TEXCOORD0;
};

cbuffer CBuniforms: register(b0)
{
    float4x4 VIEWPORT;
    float2 CAMERA;
};
int2 CAMERA_VEC: register(c0);

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
    vert = ceil(vert * VIEWPORT[2][2] + float4(CAMERA * CAMERA_VEC, 0, 0));
    vert.w = 1;
    vert = mul(VIEWPORT, vert);

    float4 color = float4(input.color)/255;

    PSinput output = {
        vert,
        color,
        VERTICES[vid].xy * input.tuv + input.toffset,
    };
    return output;
}

PSinput D2main(D2input input)
{
    float4 vert = float4(input.position, 0, 1);
    vert.xy = ceil(vert.xy * VIEWPORT[2][2]);
    vert.xy += CAMERA * CAMERA_VEC;
    vert = mul(VIEWPORT, vert);

    float4 color = float4(input.color)/255;
    PSinput output = {
        vert,
        color,
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