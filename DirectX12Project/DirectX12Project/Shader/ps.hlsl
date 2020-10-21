struct Out
{
	float4 svpos : SV_POSITION;
	float4 pos : POSITION;
};

float4 PS( Out o) : SV_TARGET
{
	return float4((o.pos.xy + float2(1, 1)) / 2, 1.0f, 1.0f);
}