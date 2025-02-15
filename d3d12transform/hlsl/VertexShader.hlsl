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

cbuffer ConstantBuffer : register(b0)
{
    float4x4 projMat;
    float4x4 viewMat;
    float4x4 worldMat;
    float4   colorMultiplier;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    matrix tMat;
    vector a1 = { 1.0f,0.0f,0.0f,0.0f};
    vector a2 = { 0.0f,1.0f,0.0f,0.0f };
    vector a3 = { 0.0f,0.0f,1.0f,0.0f };
    vector a4 = { 0.0f,0.0f,0.0f,1.0f };
    tMat[0] = a1;
    tMat[1] = a2;
    tMat[2] = a3;
    tMat[3] = a4;

    float1x4 pos = mul(float4(input.pos, 1.0f), transpose(worldMat));
    pos = mul(pos, tMat);
    pos = mul(pos, transpose(viewMat));
    output.pos = mul(pos, transpose(projMat));
    output.color = input.color * colorMultiplier;
    return output;
}