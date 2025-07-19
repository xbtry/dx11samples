cbuffer MatrixBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
};

Texture2D tex : register(t0); // Texture bound to the pixel shader
SamplerState samplerState : register(s0); // Sampler state bound to the pixel shader

struct VS_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

VS_OUTPUT VSMain(VS_INPUT input)
{
    VS_OUTPUT output;

    float4 worldPos = mul(float4(input.position, 1.0f), world);
    float4 viewPos = mul(worldPos, view);
    output.position = mul(viewPos, projection);

    output.texcoord = input.texcoord;
    output.color = input.color;
    return output;
}

float4 PSMain(VS_OUTPUT input) : SV_TARGET
{
    // Scale UV around center (0.5, 0.5)
    float2 centeredUV = input.texcoord - float2(0.5f, 0.5f);
    float2 scaledUV = centeredUV * 2;
    float2 finalUV = scaledUV + float2(0.5f, 0.5f);
    
    
    float4 texColor = tex.Sample(samplerState, finalUV);
    return texColor * input.color;
}
