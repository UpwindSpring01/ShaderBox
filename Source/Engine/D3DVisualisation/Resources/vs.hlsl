#include "SB_Header.hlsli"

struct V_INPUT
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
};

struct V_OUTPUT
{
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
};

V_OUTPUT main(V_INPUT input)
{
	V_OUTPUT output;
	output.Position = mul(float4(input.Position, 1), gWorldViewProj);
	output.Normal = input.Normal;
	return output;
}