cbuffer cb0
{
	float4	Color : packoffset(c0);
	float	fTime : packoffset(c1.y);
};

struct VS_OUTPUT
{
	float4 Position   : SV_Position;
	float4 Diffuse    : COLOR0;
};

VS_OUTPUT VS(float3 p : POSITION)
{
	VS_OUTPUT Output;
	float3 vPos = p * fTime;
	Output.Position = float4(vPos, 1.0f);
	Output.Diffuse = Color;
	return Output;
}
