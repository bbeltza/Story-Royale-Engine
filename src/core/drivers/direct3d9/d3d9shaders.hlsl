// All shader functions for the Direct3D 9 render driver
// They're nearly identical to the shaders used in Direct3D 11 (and 12), the difference being that it simply takes in the vertex positions
//      directly from the vertex shader in draw1, and it doesn't use constant buffers
//
// Use these commands to compile the shaders into a C-style array containing the bytecode for it:
//
//  fxc /T ps_2_0 /E PSmain /Fh ps.h d3d9shaders.hlsl
//  fxc /T vs_2_0 /E D1main /Fh vs1.h d3d9shaders.hlsl
//  fxc /T vs_2_0 /E D2main /Fh vs2.h d3d9shaders.hlsl

struct D1input
{
    float4 vpos: POSITION;

    float4 color: COLOR;
    float4 transform: POSITION1;
    float2 anchor: POSITION2;
    float angle: PSIZE;
    float4 texcoord: TEXCOORD;
};

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
    float2 uv: TEXCOORD;
};

float4x4 VIEWPORT: register(c0);
float2 CAMERA: register(c4);

PSinput D1main(D1input input)
{
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

    float4 vert = input.vpos - float4(input.anchor, 0.0, 0.0);
    vert = mul(vert, mul(transform, rotation));
    vert.xy = ceil(vert.xy*VIEWPORT[2][2] + (input.vpos.xy));
    vert.xy += CAMERA;
    vert = mul(VIEWPORT, vert);

    PSinput output = {
        input.color,
        vert,
        input.vpos.xy * input.texcoord.xy + input.texcoord.zw
    };
    return output;
}

PSinput D2main(D2input input)
{
    float4 vert = float4(input.pos, 0.0f, 1.0f);
    vert.xy /= 300;

    PSinput output = {
        input.color,
        vert,
        input.uv
    };
    return output;
}

sampler2D tex: register(s0);

float4 PSmain(PSinput input): COLOR
{
    return tex2D(tex, input.uv) * input.color;
}