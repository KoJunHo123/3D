#include "Shader_Engine_Defines.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_Texture;

float4 g_vColor;



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

VS_OUT VS_SMOKE_MAIN( /*����*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

	/* ������ ��ġ�� ���� �� ������ȯ�Ѵ�.*/		
	/* Ŭ���̾�Ʈ���� ���������� �����ϴ� TrnasformCoord�Լ��ʹ� �ٸ��� */
	/* mul�Լ��� ��쿡�� �����ϰ� ����� ���ϱ⸸ ������ �ϰ� w�����⿬����ü�� �������� �ʴ´�. */
    matrix TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	/* ���û��¿����� ������ ����. */
    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);

	/* ���忡 Ư�� ���·� ��ġ�Ѵ�. */
    vPosition = mul(vPosition, g_WorldMatrix);

    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);

    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;
    Out.vLifeTime = In.vLifeTime;
	
    if (In.vColor.r >= In.vColor.g && In.vColor.r >= In.vColor.b)
    {
        Out.vColor.r = 1.f;
        Out.vColor.gb = float2(0.f, 0.f);
    }
    else if (In.vColor.g >= In.vColor.r && In.vColor.g >= In.vColor.b)
    {
        Out.vColor.g = 1.f;
        Out.vColor.rb = float2(0.f, 0.f);
    }
    else
    {
        Out.vColor.b = 1.f;
        Out.vColor.rg = float2(0.f, 0.f);
    }
    Out.vColor.a = 0;
	
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
	
    Out.vColor = Out.vColor * In.vColor;
    Out.vColor.a = max(max(Out.vColor.r, Out.vColor.g), Out.vColor.b);
	
    Out.vColor.a = Out.vColor.a * (1 - (In.vLifeTime.y / In.vLifeTime.x));
	
    if (Out.vColor.a < 0.1f)
        discard;
	
    Out.vColor.rgb = g_vColor.rgb;
	
	
    return Out;
}

PS_OUT PS_BOSS_TELEPORT_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
	
    Out.vColor = Out.vColor * In.vColor;
    Out.vColor.a = max(max(Out.vColor.r, Out.vColor.g), Out.vColor.b);
	
    Out.vColor.a = Out.vColor.a * (1 - (In.vLifeTime.y / In.vLifeTime.x));
	
    Out.vColor.a *= 3.f;
    
    if (Out.vColor.a < 0.1f)
        discard;
	
    Out.vColor.rgb = g_vColor.rgb * Out.vColor.a;
	
    return Out;
}

PS_OUT PS_STONE_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float2 start = (float2) 0;
    float2 over = (float2) 0;
	
    int iTexIndex = (int) (In.vColor.r * 100) % 16;
    
    start.x = (0.25f) * iTexIndex;
    start.y = (0.25f) * (int) (iTexIndex / 4);
	
    over.x = start.x + (0.25f);
    over.y = start.y + (0.25f);
	
    float2 vTexcoord = start + (over - start) * In.vTexcoord;
	
    Out.vColor = g_Texture.Sample(LinearSampler, vTexcoord);

    if (Out.vColor.a <= 0.1f)
        discard;
	
    if (In.vLifeTime.y >= In.vLifeTime.x)
        discard;
    
    Out.vColor.rgb = (float3) 0.f;
    
    return Out;
}

PS_OUT PS_BLENDSMOKE_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float2 start = (float2) 0;
    float2 over = (float2) 0;
	
    int iTexIndex = (int) (In.vColor.r * 100) % 4;
    
    start.x = (0.5f) * iTexIndex;
    start.y = (0.5f) * (int) (iTexIndex / 2);
	
    over.x = start.x + (0.5f);
    over.y = start.y + (0.5f);
	
    float2 vTexcoord = start + (over - start) * In.vTexcoord;
	
    Out.vColor = g_Texture.Sample(LinearSampler, vTexcoord);

    Out.vColor.a = Out.vColor.r;
	
    Out.vColor.a *= 1 - (In.vLifeTime.y / In.vLifeTime.x);
    Out.vColor.a *= 2.f;
	
    if (Out.vColor.a < 0.5f)
        discard;
	
    float fRatio = 2 * (Out.vColor.a - 0.5f);
    Out.vColor.rgb = g_vColor * fRatio;
    
    return Out;
}


technique11	DefaultTechnique
{
	/* ������ + ������Ʈ + ssao + ��ָ��� + pbr*/
	pass UI // 0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

    pass SMOKE // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_SMOKE_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SMOKE_MAIN();
    }

    pass BOSS_TELEPORT // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_SMOKE_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BOSS_TELEPORT_MAIN();
    }

    pass STONE // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_STONE_MAIN();
    }

    pass BLENDSMOKE // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLENDSMOKE_MAIN();
    }
}