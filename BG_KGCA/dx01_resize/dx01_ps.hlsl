// VS_OUTPUT == PS_INPUT
struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 col : COLOR;
};

float4 PS(PS_INPUT input) : SV_TARGET
{
	return input.col;
}
