struct VS_INPUT
{
    float3 pos : POSITION;
    float4 color: COLOR;
};

struct VS_OUTPUT
{
    float4 pos: SV_POSITION;
    float4 color: COLOR;
};
cbuffer ObjectConstants: register(b0)
{
    float4x4 worldViewProj;
};
VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.pos =mul(float4(input.pos, 1.0f), transpose(worldViewProj));
    output.color = input.color;
    return output;
}