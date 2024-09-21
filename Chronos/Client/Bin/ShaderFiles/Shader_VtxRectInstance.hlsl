#include "Shader_Engine_Defines.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_Texture;

float g_fRatio;

struct VS_IN
{
	/* InputSlot : 0 */
	float3 vPosition : POSITION;
	float2 vTexcoord : TEXCOORD0;		

	/* InputSlot : 1 */
	float4 vRight : TEXCOORD1;
	float4 vUp : TEXCOORD2;
	float4 vLook : TEXCOORD3;
	float4 vTranslation : TEXCOORD4;
	float2 vLifeTime : TEXCOORD5;
    float4 vColor : COLOR0;
};

struct VS_OUT
{
	/* SV_ : ShaderValue */
	/* ���� �ؾ��� ������ ��� ���ĳ��� ���̹Ƿ� ���� dx�� �߰������� �ؾ��� �̸��� �ض�. */
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;
	float2 vLifeTime : TEXCOORD1;
    float4 vColor : COLOR0;
};

/* 1. ������ ��ȯ������ �����Ѵ�. */
/* 2. ������ ���� ������ �����Ѵ�. */
VS_OUT VS_MAIN(/*����*/VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;	

	/* ������ ��ġ�� ���� �� ������ȯ�Ѵ�.*/		
	/* Ŭ���̾�Ʈ���� ���������� �����ϴ� TrnasformCoord�Լ��ʹ� �ٸ��� */
	/* mul�Լ��� ��쿡�� �����ϰ� ����� ���ϱ⸸ ������ �ϰ� w�����⿬����ü�� �������� �ʴ´�. */
	matrix		TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	/* ���û��¿����� ������ ����. */
	vector		vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);

	/* ���忡 Ư�� ���·� ��ġ�Ѵ�. */
	vPosition = mul(vPosition, g_WorldMatrix);

	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;
	Out.vTexcoord = In.vTexcoord;
	Out.vLifeTime = In.vLifeTime;
    Out.vColor = In.vColor;
	
	return Out;
}

/* Triangle : ���� ������ �� vs_main�� ����Ҷ����� ��� */
/* ������ ��δ� ����Ǹ�. ���� ������ ����. */
/* ���ϵ� ������ w�� ������ xyzw�� ������. ���� */
/* ������ ��ġ�� ����Ʈ�� ��ȯ��. (��������ǥ�� ��ȯ�Ѵ�)*/
/* �����Ͷ����� : ���������� ��������Ͽ� �ȼ��� ���������. */

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;	
	float2 vLifeTime : TEXCOORD1;
    float4 vColor : COLOR0;
};

struct PS_OUT
{
	vector vColor : SV_TARGET0;
};


/* 1. �ȼ��� �������� ������ �����Ѵ�. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;	
	
	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

	if (Out.vColor.a <= 0.3f)
		discard;

	// ���������� ����.
	//Out.vColor.rgb = (float3)0.f;
	
    Out.vColor = In.vColor;
	//// ���� ��������.
	//Out.vColor.r = In.vLifeTime.y / In.vLifeTime.x;
	Out.vColor.a = Out.vColor.a * (1.f - (In.vLifeTime.y / In.vLifeTime.x));
	
	if (In.vLifeTime.y >= In.vLifeTime.x)
		discard;

	return Out;
}

/* 1. �ȼ��� �������� ������ �����Ѵ�. */
PS_OUT PS_SMOKE_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    if (In.vColor.r >= In.vColor.g && In.vColor.r >= In.vColor.b)
    {
        Out.vColor.r *= 1.5f;
        Out.vColor.a = Out.vColor.r;
        Out.vColor.r = 1.f - Out.vColor.r;
        Out.vColor.gb = float2(Out.vColor.r, Out.vColor.r);
    }
    else if (In.vColor.g >= In.vColor.r && In.vColor.g >= In.vColor.b)
    {
        Out.vColor.g *= 1.5f;
        Out.vColor.a = Out.vColor.g;
        Out.vColor.g = 1.f - Out.vColor.g;
        Out.vColor.rb = float2(Out.vColor.g, Out.vColor.g);
    }
    else
    {
        Out.vColor.b *= 1.5f;
        Out.vColor.a = Out.vColor.b;
        Out.vColor.b = 1.f - Out.vColor.b;
        Out.vColor.rg = float2(Out.vColor.b, Out.vColor.b);
    }
	
	if(Out.vColor.a < g_fRatio)
        discard;
	
    return Out;
}

technique11	DefaultTechnique
{
	/* ������ + ������Ʈ + ssao + ��ָ��� + pbr*/
	pass UI
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

    pass SMOKE
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SMOKE_MAIN();
    }

	/* ����� + ���� */
	//pass Effect
	//{
	//	VertexShader = compile vs_5_0 VS_MAIN_Special();
	//	PixelShader = compile ps_5_0 PS_MAIN_Special();
	//}
}