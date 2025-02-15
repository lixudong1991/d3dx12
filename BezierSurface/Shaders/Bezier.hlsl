//=============================================================================
// WaveSim.hlsl by Frank Luna (C) 2011 All Rights Reserved.
//
// UpdateWavesCS(): Solves 2D wave equation using the compute shader.
//
// DisturbWavesCS(): Runs one thread to disturb a grid height and its
//     neighbors to generate a wave. 
//=============================================================================

// For updating the simulation.
cbuffer cbUpdateSettings
{
	float4x4	Mb;
	float4x4	dMb;
	float4x4	G1;
	float4x4	G2;
	float4x4	G3;
	float4x4	G4;
};
struct Data{
	float4 posL;
	float4 normalL;
 };
RWStructuredBuffer<Data>  gBuff   : register(u0);
#define N 256
[numthreads(16, 16,1)]
void bezierCS(int3 dispatchThreadID : SV_DispatchThreadID)
{
	// We do not need to do bounds checking because:
	//	 *out-of-bounds reads return 0, which works for us--it just means the boundary of 
	//    our water simulation is clamped to 0 in local space.
	//   *out-of-bounds writes are a no-op.
	
	int x = dispatchThreadID.x;
	int y = dispatchThreadID.y;
	int index = x * N + y;
	float s = (float)x / (N-1),t = (float)y / (N - 1);

	float4 us = float4(1.0f,s,pow(s,2), pow(s, 3));
	float4 dus = mul(us, dMb);
	float4 ut = float4(1.0f, t, pow(t, 2), pow(t, 3));
	float4 dut = mul(ut, dMb);
	ut = mul(ut, Mb);
	us = mul(us, Mb);
	
	float4x4 utmbt;
	utmbt[0] = mul(ut, G1);
	utmbt[1] = mul(ut, G2);
	utmbt[2] = mul(ut, G3);
	utmbt[3] = mul(ut, G4);
	gBuff[index].posL = mul(us, utmbt);

	float4 dpus = mul(dus, utmbt);

	utmbt[0] = mul(dut, G1);
	utmbt[1] = mul(dut, G2);
	utmbt[2] = mul(dut, G3);
	utmbt[3] = mul(dut, G4);

	float4 dput = mul(us, utmbt);

	gBuff[index].normalL = float4(normalize(-cross(dpus.xyz, dput.xyz)),0.0f);
}
