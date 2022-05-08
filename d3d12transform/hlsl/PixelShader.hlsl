struct PS_OUTPUT
{
    float4 pos: SV_POSITION;
    float4 color: COLOR;
};

float4 main(PS_OUTPUT input) : SV_TARGET
{
    // return interpolated color
    return input.color;
}