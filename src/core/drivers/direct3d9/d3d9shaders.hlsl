// All shader functions for the Direct3D 9 render driver
// Use this command to compile them into a C-style array containing the bytecode for it:
//
//  fxc /T ps_2_0 /E PSmain /Fh ps.h d3d9shaders.hlsl
//  fxc /T vs_2_0 /E D1main /Fh vs1.h d3d9shaders.hlsl
//  fxc /T vs_2_0 /E D2main /Fh vs2.h d3d9shaders.hlsl

struct D2input
{
    float4 color: COLOR;
    float2 pos: POSITION;
    float2 uv: TEXCOORD;
};

struct PSinput
{
    float4 color: COLOR;
    float4 position: POSITION;
};

float4 D1main(float4 ipos: POSITION): POSITION
{
    return ipos;
}

PSinput D2main(D2input input)
{
    float4 vert = float4(input.pos, 0.0f, 1.0f);
    vert.xy /= 300;

    PSinput output = {
        input.color,
        vert
    };
    return output;
}

float4 PSmain(PSinput input): COLOR
{
    return input.color;
}