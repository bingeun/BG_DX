////////////////////////////////
cbuffer MatrixBuffer
{
	matrix matWorld;
	matrix matView;
	matrix matProj;
};

////////////////////////////////
struct VS_INPUT
{
	float4 pos : POSITION;
	float4 col : COLOR;
	float2 tex : TEXCOORD0;
	float3 norm : NORMAL;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 col : COLOR;
	float2 tex : TEXCOORD0;
	float3 norm : NORMAL;
};

////////////////////////////////
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;

	/*
	output.pos = mul(input.pos, matWorld);
	output.pos = mul(output.pos, matView);
	output.pos = mul(output.pos, matProj);
	*/
	output.pos = input.pos;
	output.col = input.col;
	output.tex = input.tex;
	output.norm = input.norm;

	return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	return input.col;
}
