#include "../.Shared/SB_Header.hlsli"

cbuffer settings : register(b2)
{
	//RADIUS of our vignette, where 0.5 results in a circle fitting the screen
    unorm float RADIUS = 0.75f;

	//softness of our vignette, between 0.0 and 1.0
    unorm float SOFTNESS = 0.45f;

    bool useSepia;
}

//sepia colour, adjust to taste
static const float3 SEPIA = float3(1.2f, 1.0f, 0.8f);

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_Target
{
	//1. VIGNETTE
    float width, height;
    gTexture.GetDimensions(width, height);

    float2 pos = (input.Position.xy / float2(width, height)) - float2(0.5f, 0.5f);
    float len = length(pos);
	//use smoothstep to create a smooth vignette
    float vignette = smoothstep(RADIUS, RADIUS - SOFTNESS, len);
	
    float3 color = gTexture.Sample(samPointWrap, input.TexCoord).rgb;
	//apply the vignette with 50% opacity
    color = lerp(color.rgb, color.rgb * vignette, 0.5);

    if (useSepia)
    {
		//2. GRAYSCALE
		//convert to grayscale using NTSC conversion weights
        float gray = dot(color.rgb, float3(0.299f, 0.587f, 0.114f));

		//3. SEPIA
		//create our sepia tone from some constant value
        float3 sepiaColor = float3(gray, gray, gray) * SEPIA;

        color = lerp(color, sepiaColor, 0.75);
    }
    
    return float4(color.rgb, 1.0f);
}