#pragma pack_matrix(column_major)

struct VIn {
    float4 position : POSITION;
#if defined(ATTRIB_TEXCOORD0)
    float2 texCoord0 : TEXCOORD;
#endif
};

struct VOut
{
    float4 position : SV_POSITION;
#if defined(ATTRIB_TEXCOORD0)
    float2 texCoord0 : TEXCOORD;
#endif
};

cbuffer VS_CONSTANT_BUFFER : register(b0) {
    float4x4 modelMatrix;
    float4x4 normalMatrix;
    float2 uvScale;
    float2 uvOffset;
    uint layer;
};

cbuffer VS_CONSTANT_BUFFER : register(b1) {
    float3 cameraPosition;
    uint2 cameraScreenSize;
    float4x4 cameraViewMatrix;
    float4x4 cameraProjectionMatrix;
};

VOut VShader(VIn input) {
    VOut output;
    float4 position_worldspace = mul(modelMatrix, input.position);
    float4 position_cameraspace = mul(cameraViewMatrix, position_worldspace);
    output.position = mul(cameraProjectionMatrix, position_cameraspace);

#if defined(ATTRIB_TEXCOORD0)
    output.texCoord0 = input.texCoord0;
#endif
    
    return output;
}

Texture2D texture0 : register(t0);
Texture2D shaderTexture : register(t0);
SamplerState SampleType;

float4 PShader(VOut input) : SV_TARGET
{
    float4 result = float4(1, 1, 1, 1);

#if defined(RESOURCE_TEXTURE0)
    float4 textureColor = shaderTexture.Sample(SampleType, input.texCoord0);
    //textureColor *= float4(3, 1, 1, 1);
    result = textureColor;
#endif

    float gamma = 2.2;
    result.rgb = pow(result.rgb, 1.0/gamma);
    return result;
}
