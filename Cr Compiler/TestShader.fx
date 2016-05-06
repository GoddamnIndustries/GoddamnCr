float4 main(float4 pos : POSITION) : SV_POSITION
{
	float a, b[(int2(1, 1) << int2(2, 3))[1]];
	return pos;
}
