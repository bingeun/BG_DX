cbuffer MatrixBuffer
{
	matrix matWorld;
	matrix matView;
	matrix matProj;
};

struct VS_INPUT
{
	float4 pos : POSITION;
	float4 col : COLOR;
};

// VS_OUTPUT == PS_INPUT
struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 col : COLOR;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;

	output.pos = mul(input.pos, matWorld);
	output.pos = mul(output.pos, matView);
	output.pos = mul(output.pos, matProj);
	output.col = input.col;

	return output;
}
