#include "SB_Header.hlsli"

InputPP main(uint vI : SV_VertexId)
{
	InputPP vout;

	// We use the 'big triangle' optimization so you only Draw 3 verticies instead of 4.
	float2 texcoord = float2((vI << 1) & 2, vI & 2);
	vout.TexCoord = texcoord;

	vout.Position = float4(texcoord.x * 2 - 1, -texcoord.y * 2 + 1, 0, 1);
	return vout;
}