cbuffer constants : register(b0)
{
    row_major float4x4 transform;
    row_major float4x4 view;
    row_major float4x4 projection;
    float3 lightvector;
}

struct vertexdesc
{
    float3 position : POS;
    float3 normal : NOR;
    float2 texcoord : TEX;
    float3 color : COL;
};

struct pixeldesc
{
    float4 position : SV_POSITION;
    float2 texcoord : TEX;
    float4 color : COL;
};

Texture2D mytexture : register(t0);
SamplerState mysampler : register(s0);

pixeldesc VertexShaderMain(vertexdesc vertex)
{
    float light = clamp(dot(mul(vertex.normal, transform), normalize(-lightvector)), 0.0f, 1.0f) * 0.8f + 0.2f;
    
    pixeldesc output;

    // Transform vertex position using World, View, and Projection matrices
    float4 worldPos = mul(float4(vertex.position, 1.0f), transform); // world position
    float4 viewPos = mul(worldPos, view); // where it is in terms of the camera space. (VIEW SPACE) 
    output.position = mul(viewPos, projection); // Transforms to clip space? What is it? Without this it like basically keeps the objects in line with the camera? like the size of the objects doesn't change as we move further away?

    // Pass through other attributes
    output.texcoord = vertex.texcoord;
    output.color = float4(vertex.color * light, 1.0f);

    return output;
}

float4 PixelShaderMain(pixeldesc pixel) : SV_TARGET
{
    float4 pixelTest = pixel.color;
    return mytexture.Sample(mysampler, pixel.texcoord) * pixelTest;
}
