struct GS_OUTPUT
{
	float4 pos : SV_POSITION;
};

[maxvertexcount(3)]
void GS(triangle float4 input[3] : SV_POSITION, inout TriangleStream< GS_OUTPUT > output)
{
	for (uint i = 0; i < 3; i++)
	{
		GSOutput element;
		element.pos = input[i];
		output.Append(element);
	}
}