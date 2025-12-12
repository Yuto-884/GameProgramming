struct VS_IN
{
    float3 pos : POSITION;
    float4 color : COLOR;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

PS_IN vs(VS_IN input)
{
    PS_IN o;
    o.pos = float4(input.pos, 1.0);
    o.color = input.color;
    return o;
}

float4 ps(PS_IN input) : SV_TARGET
{
    return input.color;
}
