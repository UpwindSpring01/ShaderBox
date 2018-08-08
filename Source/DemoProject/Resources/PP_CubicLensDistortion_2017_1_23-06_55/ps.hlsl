#include "../.Shared/SB_Header.hlsli"

Texture2D myTexture : register(t1);

cbuffer Distortion : register(b2)
{
	float gLensDistortion <string UIWidget="Slider"; float UIMin=-50; float UIMax=50;> = 0;
	float gCubicDistortion <string UIWidget="Slider"; float UIMin=-50; float UIMax=50;> = 0;
};

struct PS_INPUT
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_Target
{

	float r2 = (input.TexCoord.x - 0.5) * (input.TexCoord.x - 0.5) + (input.TexCoord.y - 0.5) * (input.TexCoord.y - 0.5);
	float f = 0;

	//only compute the cubic distortion if necessary 
	if (gCubicDistortion == 0.0)
	{
		f = 1 + r2 * gLensDistortion;
	}
	else
	{
		f = 1 + r2 * (gLensDistortion + gCubicDistortion * sqrt(r2));
	};

	// get the right pixel for the current position
	float x = f * (input.TexCoord.x - 0.5) + 0.5;
	float y = f * (input.TexCoord.y - 0.5) + 0.5;
	float3 inputDistord = myTexture.Sample(samPointClamp, float2(x, y)).rgb;

	return float4(inputDistord.r, inputDistord.g, inputDistord.b, 1);
}