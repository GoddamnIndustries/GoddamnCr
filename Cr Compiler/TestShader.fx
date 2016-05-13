#define m(a,b) a, b
#undef n

inline float b()
{
	return 34;
}
float4 main(float4 pos : POSITION) : SV_POSITION
{
	switch (1)
	{
	case 2.0f:
		if (pos.x != 0)
			return float4(m(1, 1), 1, 1);
		else
			continue;
			break;
	}

	pos *= pos[1];
	return pos;
}
