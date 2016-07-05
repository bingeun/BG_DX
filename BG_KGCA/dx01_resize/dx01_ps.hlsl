//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4 Position   : SV_Position;   // vertex position 
	float4 Diffuse    : COLOR0;     // vertex diffuse color
};

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target
{
	return input.Diffuse;
}
