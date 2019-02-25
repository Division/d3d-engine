struct VIn {
    float4 position : POSITION;
    float2 texCoord0 : TEXCOORD;
};

struct VOut
{
    float4 position : SV_POSITION;
    float2 texCoord0 : TEXCOORD;
};

cbuffer VS_CONSTANT_BUFFER : register(b0)
{
    float4x4 modelMatrix;
};

VOut VShader(VIn input)
{
    VOut output;

    output.position = mul(input.position, modelMatrix);
    output.texCoord0 = input.texCoord0;
    
    return output;
}

Texture2D texture0 : register(t0);
Texture2D shaderTexture : register(t0);
SamplerState SampleType;

float4 PShader(VOut input) : SV_TARGET
{
    float4 textureColor = shaderTexture.Sample(SampleType, input.texCoord0);
    return textureColor;
}
