Texture2D shaderTexture : register(t0);
SamplerState samplerState : register(s0);

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    float4 texColor = shaderTexture.Sample(samplerState, input.texcoord);
    if (texColor.a == 0) // or some condition to detect invalid texture
        return input.color; // fallback to vertex color
    return texColor;
}
