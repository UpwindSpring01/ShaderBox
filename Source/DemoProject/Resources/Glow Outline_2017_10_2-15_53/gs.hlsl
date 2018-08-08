#include "../.Shared/SB_Header.hlsli"
#include "Header.hlsli"

static const float creaseBias = 0.001f;

cbuffer NPR_DATA : register(b2)
{
	float creaseWidth <float UIMin=0; float UIMax=1; float UIStep=0.05;>;
	float edgeWidth <float UIMin=0; float UIMax=1; float UIStep=0.05;>;
	float creaseThreshold <float UIMin=0; float UIMax=1; float UIStep=0.1;>;
	bool useSilhoutte0;
};


void CreateVertex(inout TriangleStream<GS_DATA> triStream, float4 pos, float3 normal, float2 texCoord, uint edge = 0, float zBias = 0.0f)
{
	GS_DATA temp = (GS_DATA)0;
	
	float4 vsPos = mul(pos, gView);
	vsPos.z -= zBias;
	temp.Position = mul(vsPos, gProjection);
	
	temp.Normal = normal;
	temp.TexCoord = texCoord;
	temp.Edge = edge;

	triStream.Append(temp);
}

void CreateVertex2(inout TriangleStream<GS_DATA> triStream, float4 pos, float4 worldPos, float3 normal, uint edge = 1)
{
	GS_DATA temp = (GS_DATA)0;
	temp.Position = pos;
	temp.Normal = normal;
	temp.TexCoord = float2(0, 0);
	temp.Edge = edge;

	triStream.Append(temp);
}

/////////////////////////////////////
// Using algorithm described here: http://www.gamasutra.com/view/feature/130067/sponsored_feature_inking_the_.php?page=2
/////////////////////////////////////
void CreaseEdge(inout TriangleStream<GS_DATA> triStream, float3 normal, float3 wsAdjFaceNormal, float4 vertex0, float4 vertex1, float3 normal0, float3 normal1)
{
	// check if dot product between face normals is lower then treshold
	int i = 0;
	if (dot(wsAdjFaceNormal, normal) < creaseThreshold)
	{
		for (i = 0; i < 2; ++i)
		{
			// extrude along vertex normals
			float4 wsPos = vertex0 + i * float4(normal0, 0) * creaseWidth;
			CreateVertex(triStream, wsPos, normal0, float2(0, 0), 2, creaseBias);
		}
		for (i = 0; i < 2; ++i)
		{
			float4 wsPos = vertex1 + i * float4(normal1, 0) * creaseWidth;
			CreateVertex(triStream, wsPos, normal1, float2(0, 0), 2, creaseBias);
		}
		triStream.RestartStrip();
	}
}

/////////////////////////////////////
// Using algorithm described here: http://www.gamasutra.com/view/feature/130067/sponsored_feature_inking_the_.php?page=2
/////////////////////////////////////
void CreateSilhouttePart(inout TriangleStream<GS_DATA> triStream, float3 viewDir, float3 normal, float3 adjNormal, float4 vertex0, float4 vertex1, float3 normal0, float3 normal1)
{
	// Check if front facing
	if (dot(normal, viewDir) < 0)
	{
		// check if backfacing
		if (dot(adjNormal, viewDir) >= 0)
		{
			for (int v2 = 0; v2 < 2; ++v2)
			{
				float4 wsPos = vertex0 + v2 * float4(normal0, 0) * edgeWidth;
				CreateVertex(triStream, wsPos, normal0, float2(0, 0), 1);
			}
			for (int v1 = 0; v1 < 2; ++v1)
			{
				float4 wsPos = vertex1 + v1 * float4(normal1, 0) * edgeWidth;
				CreateVertex(triStream, wsPos, normal1, float2(0, 0), 1);
			}
			triStream.RestartStrip();
		}
		else
		{
			CreaseEdge(triStream, normal, adjNormal, vertex0, vertex1, normal0, normal1);
		}
	}
}

void Silhouette(inout TriangleStream<GS_DATA> triStream, float3 normal, float4 vertex0, float4 vertex1, float4 vertex2, float4 vertex3, float4 vertex4, float4 vertex5, float3 normal0, float3 normal2, float3 normal4)
{
	float3 viewDirection1 = normalize(vertex0.xyz - gViewInverse[3].xyz);
	float3 viewDirection2 = normalize(vertex4.xyz - gViewInverse[3].xyz);

	float3 wsAdjFaceNormal = normalize(cross(vertex1.xyz - vertex0.xyz, vertex2.xyz - vertex0.xyz));
	CreateSilhouttePart(triStream, viewDirection1, normal, wsAdjFaceNormal, vertex0, vertex2, normal0, normal2);
	wsAdjFaceNormal = normalize(cross(vertex3.xyz - vertex2.xyz, vertex4.xyz - vertex2.xyz));
	CreateSilhouttePart(triStream, viewDirection2, normal, wsAdjFaceNormal, vertex2, vertex4, normal2, normal4);
	wsAdjFaceNormal = normalize(cross(vertex5.xyz - vertex4.xyz, vertex0.xyz - vertex4.xyz));
	CreateSilhouttePart(triStream, viewDirection2, normal, wsAdjFaceNormal, vertex4, vertex0, normal4, normal0);

}

/////////////////////////////////////
// Using partially algorithm described here: http://cgstarad.com/Docs/GSContours.pdf
/////////////////////////////////////
void CreateSilhouttePart2(inout TriangleStream<GS_DATA> triStream, float3 viewDir, float3 normal, float3 adjNormal, float4 vertex0, float4 vertex1, float3 normal0, float3 normal1)
{
	// Check if front facing
	if (dot(normal, viewDir) < 0)
	{
		// check if backfacing
		if (dot(adjNormal, viewDir) >= 0)
		{
			float4 startPos = mul(vertex0, gView);
			startPos = mul(startPos, gProjection);
			float4 endPos = mul(vertex1, gView);
			endPos = mul(endPos, gProjection);

			float3 dir = endPos.xyz - startPos.xyz;
			float4 perpendicular = float4(normalize(cross(dir, float3(0, 0, -1))) * edgeWidth, 0);

			float4 n0 = normalize(mul(float4(normal0, 0), mul(gView, gProjection))) * edgeWidth;
			float4 n1 = normalize(mul(float4(normal1, 0), mul(gView, gProjection))) * edgeWidth;

			CreateVertex2(triStream, startPos + n0, vertex0, normal0);

			CreateVertex2(triStream, startPos + perpendicular, vertex0, normal0);
			CreateVertex2(triStream, startPos, vertex0, normal0);

			CreateVertex2(triStream, endPos + perpendicular, vertex1, normal1);
			CreateVertex2(triStream, endPos, vertex1, normal1);

			CreateVertex2(triStream, endPos + n1, vertex1, normal1);
			triStream.RestartStrip();
		}
		else
		{
			CreaseEdge(triStream, normal, adjNormal, vertex0, vertex1, normal0, normal1);
		}
	}
}

void Silhouette2(inout TriangleStream<GS_DATA> triStream, float3 normal, float4 vertex0, float4 vertex1, float4 vertex2, float4 vertex3, float4 vertex4, float4 vertex5, float3 normal0, float3 normal2, float3 normal4)
{
	float3 viewDirection1 = normalize(vertex0.xyz - gViewInverse[3].xyz);
	float3 viewDirection2 = normalize(vertex4.xyz - gViewInverse[3].xyz);

	float3 wsAdjFaceNormal = normalize(cross(vertex1.xyz - vertex0.xyz, vertex2.xyz - vertex0.xyz));
	CreateSilhouttePart2(triStream, viewDirection1, normal, wsAdjFaceNormal, vertex0, vertex2, normal0, normal2);
	wsAdjFaceNormal = normalize(cross(vertex3.xyz - vertex2.xyz, vertex4.xyz - vertex2.xyz));
	CreateSilhouttePart2(triStream, viewDirection2, normal, wsAdjFaceNormal, vertex2, vertex4, normal2, normal4);
	wsAdjFaceNormal = normalize(cross(vertex5.xyz - vertex4.xyz, vertex0.xyz - vertex4.xyz));
	CreateSilhouttePart2(triStream, viewDirection2, normal, wsAdjFaceNormal, vertex4, vertex0, normal4, normal0);
}

[maxvertexcount(21)]
void main(triangleadj VS_OUTPUT vertices[6], inout TriangleStream<GS_DATA> triStream)
{
	float4 vertex0 = vertices[0].Position;
	float4 vertex2 = vertices[2].Position;
	float4 vertex4 = vertices[4].Position;

	float4 vertex1 = vertices[1].Position;
	float4 vertex3 = vertices[3].Position;
	float4 vertex5 = vertices[5].Position;

	float3 normal0 = vertices[0].Normal;
	float3 normal2 = vertices[2].Normal;
	float3 normal4 = vertices[4].Normal;

	CreateVertex(triStream, vertex0, normal0, vertices[0].TexCoord);
	CreateVertex(triStream, vertex2, normal2, vertices[2].TexCoord);
	CreateVertex(triStream, vertex4, normal4, vertices[4].TexCoord);
	triStream.RestartStrip();

	float3 normal = normalize(cross(vertex2.xyz - vertex0.xyz, vertex4.xyz - vertex0.xyz));

	if (useSilhoutte0 == true)
	{
		Silhouette(triStream, normal, vertex0, vertex1, vertex2, vertex3, vertex4, vertex5, normal0, normal2, normal4);
	}
	else
	{
		Silhouette2(triStream, normal, vertex0, vertex1, vertex2, vertex3, vertex4, vertex5, normal0, normal2, normal4);
	}
}