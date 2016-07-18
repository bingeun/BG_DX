// VS_OUTPUT == PS_INPUT
struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 col : COLOR;
	float2 tex : TEXCOORD0;
	float3 norm : NORMAL;
};

float4 PS(PS_INPUT input) : SV_TARGET
{
	return input.col;
}
