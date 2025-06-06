struct vs_in{
    float3 position_local : POS0;
    float3 color : COL0;
    float2 TextCoord : TXTC0;
};
struct vs_out {
    float4 positionClip : SV_POSITION;
    float3 color : COL0;
    float2 coord : TXTC0;
};
Texture2D myTexture : register(t0);
SamplerState mySampler : register(s0);
cbuffer TestBuffer : register(b0){
    matrix transform;
};
vs_out vs_main(vs_in input){
    vs_out output = (vs_out)0;
    output.positionClip = mul(float4(input.position_local.x, input.position_local.y, input.position_local.z, 1.0f), transform);
    output.color = input.color;
    output.coord = input.TextCoord;
    return output;
}
float4 ps_main(vs_out input) : SV_TARGET{
    return myTexture.Sample(mySampler, input.coord);
}
