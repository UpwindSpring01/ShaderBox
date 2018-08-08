// Same for every draw call in frame
cbuffer BaseConstantBufferFrame : register(b0)
{
	float4x4 gView : VIEW;
	float4x4 gProjection : PROJECTION;
	float4x4 gViewInverse : VIEWINVERSE;
	float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);
};

cbuffer BaseConstantBufferObject : register(b1)
{
	float4x4 gWorld : WORLD;
	float4x4 gWorldViewProj : WORLDVIEWPROJECTION;
	float4x4 gWorldInverse : WORLDINVERSE;
};

SamplerState samLinearWrap : register(s0);
SamplerState samLinearMirror : register(s1);
SamplerState samLinearClamp : register(s2);
SamplerState samPointWrap : register(s3);
SamplerState samPointMirror : register(s4);
SamplerState samPointClamp : register(s5);