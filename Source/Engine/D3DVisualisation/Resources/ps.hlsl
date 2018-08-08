#include "SB_Header.hlsli"

struct V_OUTPUT
{
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
};

float4 main(V_OUTPUT input) : SV_TARGET
{
	float3 color_rgb = float3(0.5, 0.5, 0.5);

	//HalfLambert Diffuse
	float diffuseStrength = dot(input.Normal, -gLightDirection);
	diffuseStrength = diffuseStrength * 0.5 + 0.5;
	diffuseStrength = saturate(diffuseStrength);
	color_rgb = color_rgb * diffuseStrength;

	return float4(color_rgb , 1.0);
}