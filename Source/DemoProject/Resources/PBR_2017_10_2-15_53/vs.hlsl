#include "../.Shared/SB_Header.hlsli"
#include "Header.hlsli"

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Position = mul(float4(input.Position, 1), gWorld);
	output.Normal = normalize(mul(input.Normal, (float3x3)gWorld));
	output.TexCoord = input.TexCoord;
	return output;
}