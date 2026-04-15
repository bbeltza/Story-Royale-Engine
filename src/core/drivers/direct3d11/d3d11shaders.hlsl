// Shaders used by d3d11 (They were the original shaders for d3d12 too)
// They are compiled and put into `d3d11shaders.cpp` as an array of bytes with the command line tool `fxc`

//      fxc /T ps_4_0 /Fh ps.h /E PSmain d3d11shaders.hlsl /Gec
//      fxc /T vs_4_0 /Fh vs.h /E VSmain d3d11shaders.hlsl /Gec

struct VSinput
{
    float4 transform: POSITION0;
    float2 anchor: POSITION1;
    uint4 color: COLOR;

    float2 tuv: TEXCOORD0;
    float2 toffset: TEXCOORD1;
};

struct PSinput
{
    float4 vposition: POSITION;
    float4 color: COLOR;

    float2 tuv: TEXCOORD0;
};

cbuffer CBuniforms: register(b0)
{
    float2 _VIEWPORT;
    float2 CAMERA;
};

PSinput VSmain(VSinput input, uint vid: SV_VertexID)
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
        input.transform.xy + CAMERA, 0, 1
    );
    
    float2 VIEWPORT = float2(320, 180);

    float4x4 projection = float4x4(
        2.0/VIEWPORT.x, 0.0, 0.0,  0.0,
        0.0, 2.0/-VIEWPORT.y, 0.0, 0.0,
        0.0, 0.0,    1.0, 0.0,
        -1.0, 1.0, 0.0,    1.0
    );

    float4x4 mat = mul(transform, projection);
    float4 vert = VERTICES[vid] - float4(input.anchor, 0.0, 0.0);
    float4 color = float4(
        input.color.r/255.0f,
        input.color.g/255.0f,
        input.color.b/255.0f,
        input.color.a/255.0f
        );

    PSinput output = {
        mul(vert, mat),
        color,
        VERTICES[vid].xy * input.tuv + input.toffset,
    };
    return output;
}

SamplerState sstate: register(s0);
Texture2D tex: register(t0);

float4 PSmain(PSinput input): COLOR
{
    return input.color;
    //return tex.Sample(sstate, input.tuv) * input.color;
}