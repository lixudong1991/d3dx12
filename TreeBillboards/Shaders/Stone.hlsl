//***************************************************************************************
// TreeSprite.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//***************************************************************************************

// Defaults for number of lights.
#ifndef NUM_DIR_LIGHTS
    #define NUM_DIR_LIGHTS 3
#endif

#ifndef NUM_POINT_LIGHTS
    #define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
    #define NUM_SPOT_LIGHTS 0
#endif

// Include structures and functions for lighting.
#include "LightingUtil.hlsl"

Texture2D	 gDiffuseMap : register(t0);


SamplerState gsamPointWrap        : register(s0);
SamplerState gsamPointClamp       : register(s1);
SamplerState gsamLinearWrap       : register(s2);
SamplerState gsamLinearClamp      : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);

// Constant data that varies per frame.
cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
	float4x4 gTexTransform;
};

// Constant data that varies per material.
cbuffer cbPass : register(b1)
{
    float4x4 gView;
    float4x4 gInvView;
    float4x4 gProj;
    float4x4 gInvProj;
    float4x4 gViewProj;
    float4x4 gInvViewProj;
    float3 gEyePosW;
    float cbPerObjectPad1;
    float2 gRenderTargetSize;
    float2 gInvRenderTargetSize;
    float gNearZ;
    float gFarZ;
    float gTotalTime;
    float gDeltaTime;
    float4 gAmbientLight;

	float4 gFogColor;
	float gFogStart;
	float gFogRange;
	float2 cbPerObjectPad2;

    // Indices [0, NUM_DIR_LIGHTS) are directional lights;
    // indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
    // indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
    // are spot lights for a maximum of MaxLights per object.
    Light gLights[MaxLights];
};

cbuffer cbMaterial : register(b2)
{
	float4   gDiffuseAlbedo;
    float3   gFresnelR0;
    float    gRoughness;
	float4x4 gMatTransform;
};
 
struct VertexIn
{
	float3 PosL    : POSITION;
	float3 NormalL : NORMAL;
	float2 TexC    : TEXCOORD;
};

struct GeoOut
{
	float4 PosH    : SV_POSITION;
	float3 PosW    : POSITION;
	float3 NormalW : NORMAL;
	float2 TexC    : TEXCOORD;
};

VertexIn VS(VertexIn vin)
{
	/*
	VertexOut vout = (VertexOut)0.0f;

	// Transform to world space.
	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.PosW = posW.xyz;

	// Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
	vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);

	// Transform to homogeneous clip space.
	vout.PosH = mul(posW, gViewProj);

	// Output vertex attributes for interpolation across triangle.
	float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform);
	vout.TexC = mul(texC, gMatTransform).xy;
	*/
	return vin;
}

void Subdivide(float radius,VertexIn invert[3],out VertexIn outvert[6])
{
	//       v1 1
	//       *
	//		/ \
	//     /   \
	//3 m0*-----*m1  4
	//	 / \   / \
	//  /   \ /   \
	// *-----*-----*
	// v0    m2     v2 2
	//  0    5
	float XM_2PI = 6.283185307f;
	float XM_PI = 3.141592654f;
	outvert[0] = invert[0];
	VertexIn m0;
	m0.NormalL = normalize(0.5f * (invert[0].PosL + invert[1].PosL));
	m0.PosL = radius * m0.NormalL;
	float theta = atan2(m0.PosL.z, m0.PosL.x);
	// Put in [0, 2pi].
	if (theta < 0.0f)
		theta += XM_2PI;

	float phi = acos(m0.PosL.y / radius);

	m0.TexC.x = theta / XM_2PI;
	m0.TexC.y = phi / XM_PI;
	//m0.TexC = 0.5f * (invert[0].TexC + invert[1].TexC);
	outvert[1] = m0;
	VertexIn m1;
	m1.NormalL = normalize(0.5f * (invert[1].PosL + invert[2].PosL));
	m1.PosL = radius * m1.NormalL;
	theta = atan2(m1.PosL.z, m1.PosL.x);
	// Put in [0, 2pi].
	if (theta < 0.0f)
		theta += XM_2PI;

	phi = acos(m1.PosL.y / radius);

	m1.TexC.x = theta / XM_2PI;
	m1.TexC.y = phi / XM_PI;
	outvert[3] = m1;
	VertexIn m2;
	m2.NormalL = normalize(0.5f * (invert[0].PosL + invert[2].PosL));
	m2.PosL = radius * m2.NormalL;
	theta = atan2(m2.PosL.z, m2.PosL.x);
	// Put in [0, 2pi].
	if (theta < 0.0f)
		theta += XM_2PI;

	phi = acos(m2.PosL.y / radius);

	m2.TexC.x = theta / XM_2PI;
	m2.TexC.y = phi / XM_PI;
	outvert[2] = m2;
	outvert[4] = invert[2];
	outvert[5] = invert[1];
	
}
void CreateGeo(VertexIn vin[6],out GeoOut gout[6])
{
	[unroll]
	for (int i = 0; i < 6; ++i)
	{
		// Transform to world space.
		float4 posW = mul(float4(vin[i].PosL, 1.0f), gWorld);
		gout[i].PosW = posW.xyz;
		// Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
		gout[i].NormalW = mul(vin[i].NormalL, (float3x3)gWorld);
		// Transform to homogeneous clip space.
		gout[i].PosH = mul(posW, gViewProj);
		// Output vertex attributes for interpolation across triangle.
		float4 texC = mul(float4(vin[i].TexC, 0.0f, 1.0f), gTexTransform);
		gout[i].TexC = mul(texC, gMatTransform).xy;
	}

}
 // We expand each point into a quad (4 vertices), so the maximum number of vertices
 // we output per geometry shader invocation is 4.
[maxvertexcount(24)]
void GS(triangle VertexIn gin[3],
        inout TriangleStream<GeoOut> triStream)
{	
	//
	// Compute the local coordinate system of the sprite relative to the world
	// space such that the billboard is aligned with the y-axis and faces the eye.
	//
	float3 centerW = mul(float4(0.0f, 0.0f, 0.0f, 1.0f), gWorld).xyz;
	float3 toEyeW = gEyePosW - centerW;
	float distToEye = length(toEyeW);
	float radius = length(gin[0].PosL);
	VertexIn outver[6];
	int i;
	if (distToEye>80.0f)
	{
		GeoOut geo;
		[unroll]
		for (i=0;i<3;i++)
		{
			// Transform to world space.
			float4 posW = mul(float4(gin[i].PosL, 1.0f), gWorld);
			geo.PosW = posW.xyz;
			// Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
			geo.NormalW = mul(gin[i].NormalL, (float3x3)gWorld);
			// Transform to homogeneous clip space.
			geo.PosH = mul(posW, gViewProj);
			// Output vertex attributes for interpolation across triangle.
			float4 texC = mul(float4(gin[i].TexC, 0.0f, 1.0f), gTexTransform);
			geo.TexC = mul(texC, gMatTransform).xy;
			triStream.Append(geo);
		}	
		return;
	}

	Subdivide(radius,gin, outver);
    if (distToEye > 40.0f)
	{
		GeoOut gout[6];
		[unroll]
		for ( i = 0; i < 6; ++i)
		{
			// Transform to world space.
			float4 posW = mul(float4(outver[i].PosL, 1.0f), gWorld);
			gout[i].PosW = posW.xyz;
			// Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
			gout[i].NormalW = mul(outver[i].NormalL, (float3x3)gWorld);
			// Transform to homogeneous clip space.
			gout[i].PosH = mul(posW, gViewProj);
			// Output vertex attributes for interpolation across triangle.
			float4 texC = mul(float4(outver[i].TexC, 0.0f, 1.0f), gTexTransform);
			gout[i].TexC = mul(texC, gMatTransform).xy;
		}
		[unroll]
		for (i = 0; i < 5; ++i)
		{
			triStream.Append(gout[i]);
		}
		triStream.RestartStrip();
		triStream.Append(gout[1]);
		triStream.Append(gout[5]);
		triStream.Append(gout[3]);
		return;
	}

	
	VertexIn a[6];
	VertexIn b[6];
	VertexIn c[6];
	VertexIn d[6];
	VertexIn invert1[3];
	invert1[0] = outver[0];
	invert1[1] = outver[1];
	invert1[2] = outver[2];
	VertexIn invert2[3];
	invert2[0] = outver[1];
	invert2[1] = outver[3];
	invert2[2] = outver[2];
	VertexIn invert3[3];
	invert3[0] = outver[2];
	invert3[1] = outver[3];
	invert3[2] = outver[4];
	VertexIn invert4[3];
	invert4[0] = outver[1];
	invert4[1] = outver[5];
	invert4[2] = outver[3];
	Subdivide(radius,invert1, a);
	Subdivide(radius,invert2, b);
	Subdivide(radius,invert3, c);
	Subdivide(radius,invert4, d);

	GeoOut aa[6];
	GeoOut bb[6];
	GeoOut cc[6];
	GeoOut dd[6];
	CreateGeo(a, aa);
	CreateGeo(b, bb);
	CreateGeo(c, cc);
	CreateGeo(d, dd);
	triStream.Append(aa[0]);
	triStream.Append(aa[1]);
	triStream.Append(aa[2]);
	triStream.Append(aa[3]);
	triStream.Append(aa[4]);
	triStream.Append(bb[3]);
	triStream.Append(cc[2]);
	triStream.Append(cc[3]);
	triStream.Append(cc[4]);
	triStream.RestartStrip();
	triStream.Append(aa[1]);
	triStream.Append(aa[5]);
	triStream.Append(aa[3]);
	triStream.Append(bb[1]);
	triStream.Append(bb[3]);
	triStream.Append(bb[5]);
	triStream.Append(cc[3]);
	triStream.RestartStrip();
	triStream.Append(aa[5]);
	triStream.Append(dd[1]);
	triStream.Append(bb[1]);
	triStream.Append(dd[3]);
	triStream.Append(bb[5]);
	triStream.RestartStrip();
	triStream.Append(dd[1]);
	triStream.Append(dd[5]);
	triStream.Append(dd[3]);
}
[maxvertexcount(3)]
void GSBoom(triangle VertexIn gin[3],
	inout TriangleStream<GeoOut> triStream)
{
	float3 v0 = gin[0].PosL;
	float3 v1 = gin[1].PosL;
	float3 v2 = gin[2].PosL;

	float3 n = v0 + v1 + v2;
	n *= (gTotalTime);

	gin[0].PosL = v0 + n;
	gin[1].PosL = v1 + n;
	gin[2].PosL = v2 + n;
	gin[0].NormalL = gin[0].NormalL + n;
	gin[1].NormalL = gin[1].NormalL + n;
	gin[2].NormalL = gin[2].NormalL + n;
	GeoOut geo;
	[unroll]
	for (int i = 0; i < 3; i++)
	{
		// Transform to world space.
		float4 posW = mul(float4(gin[i].PosL, 1.0f), gWorld);
		geo.PosW = posW.xyz;
		// Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
		geo.NormalW = mul(gin[i].NormalL, (float3x3)gWorld);
		// Transform to homogeneous clip space.
		geo.PosH = mul(posW, gViewProj);
		// Output vertex attributes for interpolation across triangle.
		float4 texC = mul(float4(gin[i].TexC, 0.0f, 1.0f), gTexTransform);
		geo.TexC = mul(texC, gMatTransform).xy;
		triStream.Append(geo);
	}
	return;
}