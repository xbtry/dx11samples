Texture2D shaderTexture : register(t0);
SamplerState samplerState : register(s0);

struct PS_INPUT
{
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    float4 textureColor = shaderTexture.Sample(samplerState, input.texcoord);
    return input.color * textureColor;
}
