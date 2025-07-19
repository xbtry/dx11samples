// shader.hlsl

cbuffer ConstantBuffer : register(b0)
{
    float4x4 worldViewProj; // not used yet
}

struct VSInput {
    float3 pos : POSITION;
    float4 color : COLOR;
};

struct PSInput {
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    output.pos = float4(input.pos, 1.0f); // no transform yet
    output.color = input.color;
    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.color;
}
