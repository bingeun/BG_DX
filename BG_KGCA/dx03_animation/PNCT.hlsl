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
	float3 nor : NORMAL;
	float4 col : COLOR;
	float2 tex : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float3 nor : NORMAL;
	float4 col : COLOR;
	float2 tex : TEXCOORD0;
};

////////////////////////////////
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;

	output.pos = mul(input.pos, matWorld);
	output.pos = mul(output.pos, matView);
	output.pos = mul(output.pos, matProj);
	output.nor = input.nor;
	output.col = input.col;
	output.tex = input.tex;

	return output;
}

Texture2D Texture0 : register(t0);
SamplerState Sampler0 : register(s0);

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	return input.col;
}

float4 PS_Tex(VS_OUTPUT input) : SV_TARGET
{
	return Texture0.Sample(Sampler0, input.tex) * input.col;
}
