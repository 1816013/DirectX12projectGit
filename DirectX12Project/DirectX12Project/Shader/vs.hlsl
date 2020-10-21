struct Out
{
	float4 svpos : SV_POSITION;
	float4 pos : POSITION;
};

Out VS( float4 pos : POSITION ) //: SV_POSITION
{
	Out o;
	o.svpos = pos;
	o.pos = pos;
	return o;
}