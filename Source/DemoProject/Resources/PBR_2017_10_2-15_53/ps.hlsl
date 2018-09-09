#include "../.Shared/SB_Header.hlsli"
#include "Header.hlsli"

cbuffer colorSettings : register(b3)
{
	float3 m_ColorDiffuse <string UIWidget="Color";>;
	
	float3 m_ColorAmbient <string UIWidget="Color";>;
	unorm float m_AmbientIntensity = 0.3f;
	
	float4 m_SilhoutteEdgeColor <string UIWidget="Color";>;
	float4 m_CreaseEdgeColor <string UIWidget="Color";>;
	
	bool useTexture;
}

Texture2D gModelTexture : register(t0);

float4 main(GS_DATA input) : SV_TARGET
{
	if (input.Edge == 1)
	{
		return m_SilhoutteEdgeColor;
	}
	else if (input.Edge == 2)
	{
		return m_CreaseEdgeColor;
	}
	
	float3 viewDirection = normalize(input.Position.xyz - gViewInverse[3].xyz);

	float diffuseStrength = dot(input.Normal, -gLightDirection);
	diffuseStrength = diffuseStrength * 0.5 + 0.5;
	diffuseStrength = saturate(diffuseStrength);
	float3 color_rgb = useTexture ? gModelTexture.Sample(samLinearWrap, float2(input.TexCoord.x, -input.TexCoord.y)).xyz : 1.0f;
	color_rgb = m_ColorAmbient.rgb * m_AmbientIntensity + color_rgb * diffuseStrength * m_ColorDiffuse;

	return float4(color_rgb, 1.0f);
}