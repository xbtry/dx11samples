cbuffer MatrixBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
};

struct VS_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VS_OUTPUT VSMain(VS_INPUT input)
{
    VS_OUTPUT output;

    float4 worldPos = mul(float4(input.position, 1.0f), world);
    float4 viewPos = mul(worldPos, view);
    output.position = mul(viewPos, projection);

    output.color = input.color;
    return output;
}

float4 PSMain(VS_OUTPUT input) : SV_TARGET
{
    return input.color;
}
