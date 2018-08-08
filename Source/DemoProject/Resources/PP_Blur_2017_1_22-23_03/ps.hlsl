#include "../.Shared/SB_Header.hlsli"

struct PS_INPUT
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_Target
{
	float width, height;
	gTexture.GetDimensions(width, height);

	float dx = 1.0f / width;
	float dy = 1.0f / height;

	float3 color = float3(0, 0, 0);
	int counter = 0;
	for (int i = -2; i < 2; ++i)
	{
		for (int j = -2; j < 2; ++j)
		{
			float2 pos = input.TexCoord + float2(i * dx * 2, j * dy * 2);
			if (pos.x >= 0 && pos.y >= 0 && pos.x <= width && pos.y <= height)
			{
				counter += 1;
				color += gTexture.Sample(samPointWrap, pos).rgb;
			}
		}
	}
	float3 finalColor = color / counter;

	return float4(finalColor, 1.0f);
}