struct VS_INPUT
{
	float4 pos : POSITION;
	float4 col : COLOR;
	float2 tex : TEXCOORD0;
};

// VS_OUTPUT == PS_INPUT
struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 col : COLOR;
	float2 tex : TEXCOORD0;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;

	output.pos = input.pos;
	output.col = input.col;
	output.tex = input.tex;

	return output;
}
