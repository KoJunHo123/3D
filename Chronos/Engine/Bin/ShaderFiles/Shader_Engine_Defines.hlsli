

// �ؽ�ó ���ø�
// ����Ʈ ���ø�
sampler PointSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = wrap;
    AddressV = wrap;
};

// ���� ���ø�
sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;    // ��ǥ ����� ��� 0���� �ٽ� ����.
    AddressV = wrap;    
};

sampler LinearClampSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = clamp;   // ��ǥ�� ����� ��� ���� ������ ���� ���� ������ ����.(0 ���� �ؿ� ������ 0����, 1���� ���� ������ 1��.)
    AddressV = clamp;
};

// �����Ͷ����� : 3D �׷����� �׸��� ���� ���� ������ �ȼ� �̹����� ��ȯ. == 3���� �ﰢ�����κ��� �ȼ� ������� ���. -> �̰� �ϰ��� �ȼ� ���̴�.
RasterizerState RS_Default  
{
    FillMode = solid;   // �ﰢ�� ���� ��� ä�� �ų�
    CullMode = back;    // ��� �κ��� �ƿ� �����ų�.
    FrontCounterClockwise = false;  // �ݽð� ������ ������ �ٲܷ�?
};

RasterizerState RS_Wireframe
{
    FillMode = wireframe;
    CullMode = back;
    FrontCounterClockwise = false;
};

RasterizerState RS_Cull_CW
{
    FillMode = solid;
    CullMode = front;
    FrontCounterClockwise = false;
};

RasterizerState RS_Cull_None
{
    FillMode = solid;
    CullMode = none;
    FrontCounterClockwise = false;
};

// ���� ���ٽ� ����
DepthStencilState DSS_Default
{
    DepthEnable = true;     // ���� �� �ҷ� ����
    DepthWriteMask = all;   // ���̰� ��� �ҷ� ����
    DepthFunc = less_equal; // ���̰� ����� �� ���� �� �� �����ϰ�. (less_equal == �۰ų� ���� ��)
};

DepthStencilState DSS_None
{
    DepthEnable = false;
    DepthWriteMask = zero;
};

// ���� ����
BlendState BS_Default
{
    BlendEnable[0] = false; // �� ���ҰԿ�.
};

BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;  // �Ұſ���. �� �迭������ ���� ����...

    SrcBlend = Src_Alpha;   // ���� ��� = Src�� ����
    DestBlend = Inv_Src_Alpha;  // ���� ��� = 1 - Src�� ����.
    BlendOp = Add;  // ���ϱ��.

	/*

	SrcBlend[0] = Src_Alpha;
	DestBlend[0] = Inv_Src_Alpha;
	BlendOp[0] = Add;

	SrcBlend[1] = one;
	DestBlend[1] = one;
	BlendOp[1] = Add;*/
};

// ������ �� 50:50���� -> ���İ��� �� 0.5�� ħ.
BlendState BS_OnebyOne
{
    BlendEnable[0] = true;

    SrcBlend = one;
    DestBlend = one;
    BlendOp = Add;
};