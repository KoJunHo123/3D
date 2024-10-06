#include "Shader_Engine_Defines.hlsli"
/* float2 float3 float4 == vector */
/* float1x3, float3x3, float1x3, float4x4 == matrix */

/* VertexShader */
/* �����ϳ��� � ��ȯ�� ���İ��� �Ǵ����� ���� ������ ���� ����. */
/* ���� ���̴��� ���� �ϳ��� �޾ƿ���. ������ ��ģ�� ������ ��ģ ������ �������ش�. */
/* �簢�����۸� �׸���. 6��ȣ��. */
// VS_MAIN(/*����*/float3 vPosition  : POSITION, float2 vTexcoord : TEXCOORD0)
//{
//
//}

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_Texture;
texture2D       g_PackTexture;

float2			g_TexDivide;
int				g_iTexIndex;

float4          g_vColor = { 1.f, 1.f, 1.f, 1.f };

float           g_fRatio;

struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
	/* SV_ : ShaderValue */
	/* ���� �ؾ��� ������ ��� ���ĳ��� ���̹Ƿ� ���� dx�� �߰������� �ؾ��� �̸��� �ض�. */
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;
	
};

/* 1. ������ ��ȯ������ �����Ѵ�. */
/* 2. ������ ���� ������ �����Ѵ�. */
VS_OUT VS_MAIN(/*����*/VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;	

	/* ������ ��ġ�� ���� �� ������ȯ�Ѵ�.*/		
	/* Ŭ���̾�Ʈ���� ���������� �����ϴ� TrnasformCoord�Լ��ʹ� �ٸ��� */
	/* mul�Լ��� ��쿡�� �����ϰ� ����� ���ϱ⸸ ������ �ϰ� w�����⿬����ü�� �������� �ʴ´�. */
	vector		vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;
	Out.vTexcoord = In.vTexcoord;

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
};

struct PS_OUT
{
	vector vColor : SV_TARGET0;
};

/* 1. �ȼ��� �������� ������ �����Ѵ�. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	
	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);/*vector(1.f, In.vTexcoord.y, 0.f, 1.f);*/	

    if (Out.vColor.a < 0.1f)
        discard;
    
	return Out;
}

/* 1. �ȼ��� �������� ������ �����Ѵ�. */
PS_OUT PS_BLENDUI_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord); /*vector(1.f, In.vTexcoord.y, 0.f, 1.f);*/

    Out.vColor.a *= g_fRatio;
    
    return Out;
}


/* 1. �ȼ��� �������� ������ �����Ѵ�. */
PS_OUT PS_COLOR_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord); /*vector(1.f, In.vTexcoord.y, 0.f, 1.f);*/

    if (Out.vColor.a < 0.1f)
        discard;
    
    Out.vColor = g_vColor;
    
    return Out;
}

PS_OUT PS_SPRITE_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float2 start = (float2) 0;
    float2 over = (float2) 0;
	
    start.x = (1 / g_TexDivide.x) * g_iTexIndex;
    start.y = (1 / g_TexDivide.y) * (int) (g_iTexIndex / g_TexDivide.x);
	
    over.x = start.x + (1 / g_TexDivide.x);
    over.y = start.y + (1 / g_TexDivide.y);
	
    float2 vTexcoord = start + (over - start) * In.vTexcoord;
	
    Out.vColor = g_Texture.Sample(LinearSampler, vTexcoord); /*vector(1.f, In.vTexcoord.y, 0.f, 1.f);*/
    
    if(Out.vColor.a < 0.1f)
        discard;
    //Out.vColor.rgb = g_vColor.rgb;
	
    return Out;
}

PS_OUT PS_FLASH_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float2 start = (float2) 0;
    float2 over = (float2) 0;
	
    start.x = (1 / g_TexDivide.x) * g_iTexIndex;
    start.y = (1 / g_TexDivide.y) * (int) (g_iTexIndex / g_TexDivide.x);
	
    over.x = start.x + (1 / g_TexDivide.x);
    over.y = start.y + (1 / g_TexDivide.y);
	
    float2 vTexcoord = start + (over - start) * In.vTexcoord;
	
    Out.vColor = g_Texture.Sample(LinearSampler, vTexcoord);
	
    Out.vColor.rgb = g_vColor.rgb * (1 - Out.vColor.a);
    
    return Out;
}

PS_OUT PS_SPARK_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float2 start = (float2) 0;
    float2 over = (float2) 0;
	
    start.x = (1 / g_TexDivide.x) * g_iTexIndex;
    start.y = (1 / g_TexDivide.y) * (int) (g_iTexIndex / g_TexDivide.x);
	
    over.x = start.x + (1 / g_TexDivide.x);
    over.y = start.y + (1 / g_TexDivide.y);
	
    float2 vTexcoord = start + (over - start) * In.vTexcoord;
	
    Out.vColor = g_Texture.Sample(LinearSampler, vTexcoord);
	
    Out.vColor.a = Out.vColor.r;
    
    if (Out.vColor.a < 0.1f)
        discard;
    
    Out.vColor.rgb = g_vColor.rgb * (1 - Out.vColor.a);
    
    return Out;
}

PS_OUT PS_FLARE_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
	
    Out.vColor.a = Out.vColor.r * saturate(g_vColor.a);
    
    Out.vColor.r = 1.f - (1 - g_vColor.r) * (1 - Out.vColor.a * 2.f);
    Out.vColor.g = 1.f - (1 - g_vColor.g) * (1 - Out.vColor.a * 2.f);
    Out.vColor.b = 1.f - (1 - g_vColor.b) * (1 - Out.vColor.a * 2.f);
        
    return Out;
}

PS_OUT PS_GAGEBAR_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord); /*vector(1.f, In.vTexcoord.y, 0.f, 1.f);*/

    if (Out.vColor.a < 0.1f)
        discard;
    
    //Out.vColor.rgb = g_vColor.rgb;
    
    float4 vPackColor = g_PackTexture.Sample(LinearSampler, In.vTexcoord);
    
    float fRatio = g_fRatio * 0.98f + 0.01f;
    
    if(0.01f < In.vTexcoord.x && In.vTexcoord.x < fRatio
        && 0.2f < In.vTexcoord.y && In.vTexcoord.y < 0.8f)
        Out.vColor = vPackColor.r * g_vColor;
    
    return Out;
}

PS_OUT PS_HPBAR_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord); /*vector(1.f, In.vTexcoord.y, 0.f, 1.f);*/
    
    if (Out.vColor.a < 0.1f)
        discard;
    
    float fRatio = g_fRatio * 0.94f + 0.03f;
    
    if (0.03f < In.vTexcoord.x && In.vTexcoord.x < fRatio
        && 0.2f < In.vTexcoord.y && In.vTexcoord.y < 0.8f)
        Out.vColor.rgb = g_vColor.rgb;
    
    return Out;
}

PS_OUT PS_BOSS_HPBAR_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord); /*vector(1.f, In.vTexcoord.y, 0.f, 1.f);*/
    
    if (Out.vColor.a < 0.1f)
        discard;
    
    Out.vColor.rgb = 1.f - Out.vColor.rgb;
    
    if (In.vTexcoord.x < g_fRatio)
        Out.vColor.rgb *= g_vColor.rgb;
    else
        Out.vColor.rgb = (float3) 0.f;
        return Out;
}


technique11	DefaultTechnique
{
	/* ������ + ������Ʈ + ssao + ��ָ��� + pbr*/
    pass UI //0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

    pass SPRITE //1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SPRITE_MAIN();
    }

    pass FLASH //2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_FLASH_MAIN();
    }

    pass SPARK //3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SPARK_MAIN();
    }

    pass FLARE //4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_FLARE_MAIN();
    }

    pass BLENDUI //5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLENDUI_MAIN();
    }

    pass COLORUI //6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_COLOR_MAIN();
    }

    pass GAGEBAR_UI //7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_GAGEBAR_MAIN();
    }

    pass HPBAR_UI //8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_HPBAR_MAIN();
    }

    pass BOSS_HPBAR_UI //9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BOSS_HPBAR_MAIN();
    }
}
