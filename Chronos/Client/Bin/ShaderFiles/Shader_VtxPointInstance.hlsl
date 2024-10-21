#include "Shader_Engine_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;
texture2D g_SubTexture;
vector g_vCamPosition;

float4 g_vColor;
float g_fRatio;
float2 g_vTexDivide;
uint g_iSkillIndex;

struct VS_IN
{
	/* InputSlot : 0 */
    float3 vPosition : POSITION;
    float2 vPSize : PSIZE;
	/* InputSlot : 1 */ 
    row_major float4x4 TransformMatrix : WORLD; //row_majo : ���� ��������.
    float2 vLifeTime : COLOR0;
    float4 vColor : COLOR1;
};

struct VS_OUT
{
	/* SV_ : ShaderValue */
	/* ���� �ؾ��� ������ ��� ���ĳ��� ���̹Ƿ� ���� dx�� �߰������� �ؾ��� �̸��� �ض�. */
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
    float2 vLifeTime : COLOR0;
    float4 vColor : COLOR1;
    float3 vLook : TEXCOORD0;
};

/* 1. ������ ��ȯ������ �����Ѵ�. */
/* 2. ������ ���� ������ �����Ѵ�. */
VS_OUT VS_MAIN( /*����*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
	/* ���û��¿����� ������ ����. */
    vector vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix);

	/* ���忡 Ư�� ���·� ��ġ�Ѵ�. */
    vPosition = mul(vPosition, g_WorldMatrix);
	
    Out.vPosition = vPosition;
    Out.vPSize = In.vPSize;
    Out.vLifeTime = In.vLifeTime;
    Out.vColor = In.vColor;
    Out.vLook = In.TransformMatrix._31_32_33;;
	
    
    return Out;
}

VS_OUT VS_SWORDPARTICLE_MAIN( /*����*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    vector vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix);
    
    Out.vPosition = vPosition;
    Out.vPSize = In.vPSize;
    Out.vLifeTime = In.vLifeTime;
    Out.vColor = In.vColor;
    Out.vLook = In.TransformMatrix._31_32_33;;
	
    
    return Out;
}


struct GS_IN
{
	/* SV_ : ShaderValue */
	/* ���� �ؾ��� ������ ��� ���ĳ��� ���̹Ƿ� ���� dx�� �߰������� �ؾ��� �̸��� �ض�. */
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
    float2 vLifeTime : COLOR0;
    float4 vColor : COLOR1;
    float3 vLook : TEXCOORD0;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : COLOR0;
    float4 vColor : COLOR1;
};

[maxvertexcount(6)] // �� ����� ��. ���� �� �� ���� ����.(�ε��� ����)
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Container)
{
    GS_OUT Out[4];

	// �������.
    float3 vLook = (g_vCamPosition - In[0].vPosition).xyz;
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f;

    Out[0].vPosition = float4(In[0].vPosition.xyz + vRight + vUp, 1.f);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;
    Out[0].vColor = In[0].vColor;

    Out[1].vPosition = float4(In[0].vPosition.xyz - vRight + vUp, 1.f);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;
    Out[1].vColor = In[0].vColor;

    Out[2].vPosition = float4(In[0].vPosition.xyz - vRight - vUp, 1.f);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    Out[2].vColor = In[0].vColor;

    Out[3].vPosition = float4(In[0].vPosition.xyz + vRight - vUp, 1.f);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;
    Out[3].vColor = In[0].vColor;

    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);

    Container.Append(Out[0]);
    Container.Append(Out[1]);
    Container.Append(Out[2]);
    Container.RestartStrip(); // ���⼭���� �ٽ� ��ڴ�. �̰� �����ָ� 123���� ������.

    Container.Append(Out[0]);
    Container.Append(Out[2]);
    Container.Append(Out[3]);
    Container.RestartStrip();
}

[maxvertexcount(6)] // �� ����� ��. ���� �� �� ���� ����.(�ε��� ����)
void GS_DIRECTION_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Container)
{
    GS_OUT Out[4];

    float3 vCamDir = normalize(g_vCamPosition - In[0].vPosition).xyz;
	// �������. : �� �������� ���� ����.
    float3 vLook = normalize(In[0].vLook);
    float3 vUp = normalize(cross(vLook, vCamDir));
    float3 vRight = normalize(cross(vUp, vLook));
	
    vLook *= In[0].vPSize.x * (1 - In[0].vLifeTime.y / In[0].vLifeTime.x);
    vUp *= In[0].vPSize.y * (1 - In[0].vLifeTime.y / In[0].vLifeTime.x);
	
    Out[0].vPosition = float4(In[0].vPosition.xyz - vLook + vUp, 1.f);
    Out[0].vTexcoord = float2(0.f, 0.0f);
    Out[0].vLifeTime = In[0].vLifeTime;
    Out[0].vColor = In[0].vColor;

    Out[1].vPosition = float4(In[0].vPosition.xyz + vLook + vUp, 1.f);
    Out[1].vTexcoord = float2(1.f, 0.0f);
    Out[1].vLifeTime = In[0].vLifeTime;
    Out[1].vColor = In[0].vColor;

    Out[2].vPosition = float4(In[0].vPosition.xyz + vLook - vUp, 1.f);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    Out[2].vColor = In[0].vColor;

    Out[3].vPosition = float4(In[0].vPosition.xyz - vLook - vUp, 1.f);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;
    Out[3].vColor = In[0].vColor;

    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);

    Container.Append(Out[0]);
    Container.Append(Out[1]);
    Container.Append(Out[2]);
    Container.RestartStrip(); // ���⼭���� �ٽ� ��ڴ�. �̰� �����ָ� 123���� ������.

    Container.Append(Out[0]);
    Container.Append(Out[2]);
    Container.Append(Out[3]);
    Container.RestartStrip();
}

[maxvertexcount(6)] // �� ����� ��. ���� �� �� ���� ����.(�ε��� ����)
void GS_SPARK_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Container)
{
    GS_OUT Out[4];

    float3 vCamDir = normalize(g_vCamPosition - In[0].vPosition).xyz;
	// �������. : �� �������� ���� ����.
    float3 vLook = normalize(In[0].vLook);
    float3 vUp = normalize(cross(vLook, vCamDir));
    float3 vRight = normalize(cross(vUp, vLook));
	
    vLook *= In[0].vPSize.x * (1 - In[0].vLifeTime.y / In[0].vLifeTime.x);
    vUp *= In[0].vPSize.y * (1 - In[0].vLifeTime.y / In[0].vLifeTime.x) * 0.25f;
	
    Out[0].vPosition = float4(In[0].vPosition.xyz - vLook + vUp, 1.f);
    Out[0].vTexcoord = float2(0.f, 0.5f);
    Out[0].vLifeTime = In[0].vLifeTime;
    Out[0].vColor = In[0].vColor;

    Out[1].vPosition = float4(In[0].vPosition.xyz + vLook + vUp, 1.f);
    Out[1].vTexcoord = float2(1.f, 0.5f);
    Out[1].vLifeTime = In[0].vLifeTime;
    Out[1].vColor = In[0].vColor;

    Out[2].vPosition = float4(In[0].vPosition.xyz + vLook - vUp, 1.f);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    Out[2].vColor = In[0].vColor;

    Out[3].vPosition = float4(In[0].vPosition.xyz - vLook - vUp, 1.f);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;
    Out[3].vColor = In[0].vColor;

    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);

    Container.Append(Out[0]);
    Container.Append(Out[1]);
    Container.Append(Out[2]);
    Container.RestartStrip(); // ���⼭���� �ٽ� ��ڴ�. �̰� �����ָ� 123���� ������.

    Container.Append(Out[0]);
    Container.Append(Out[2]);
    Container.Append(Out[3]);
    Container.RestartStrip();
}

[maxvertexcount(6)] // �� ����� ��. ���� �� �� ���� ����.(�ε��� ����)
void GS_SPARK_DIR_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Container)
{
    GS_OUT Out[4];

    float3 vCamDir = normalize(g_vCamPosition - In[0].vPosition).xyz;
	// �������. : �� �������� ���� ����.
    float3 vLook = normalize(In[0].vLook);
    float3 vUp = normalize(cross(vLook, vCamDir));
    float3 vRight = normalize(cross(vUp, vLook));
	
    vLook *= In[0].vPSize.x;
    vUp *= In[0].vPSize.y * 0.25f;
	
    Out[0].vPosition = float4(In[0].vPosition.xyz - vLook + vUp, 1.f);
    Out[0].vTexcoord = float2(0.f, 0.5f);
    Out[0].vLifeTime = In[0].vLifeTime;
    Out[0].vColor = In[0].vColor;

    Out[1].vPosition = float4(In[0].vPosition.xyz + vLook + vUp, 1.f);
    Out[1].vTexcoord = float2(1.f, 0.5f);
    Out[1].vLifeTime = In[0].vLifeTime;
    Out[1].vColor = In[0].vColor;

    Out[2].vPosition = float4(In[0].vPosition.xyz + vLook - vUp, 1.f);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    Out[2].vColor = In[0].vColor;

    Out[3].vPosition = float4(In[0].vPosition.xyz - vLook - vUp, 1.f);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;
    Out[3].vColor = In[0].vColor;

    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);

    Container.Append(Out[0]);
    Container.Append(Out[1]);
    Container.Append(Out[2]);
    Container.RestartStrip(); // ���⼭���� �ٽ� ��ڴ�. �̰� �����ָ� 123���� ������.

    Container.Append(Out[0]);
    Container.Append(Out[2]);
    Container.Append(Out[3]);
    Container.RestartStrip();
}

[maxvertexcount(6)] // �� ����� ��. ���� �� �� ���� ����.(�ε��� ����)
void GS_SWORDPARTICLE_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Container)
{
    GS_OUT Out[4];

    float fTime = 1 - (In[0].vLifeTime.y / In[0].vLifeTime.x);
	// �������.
    float3 vLook = (g_vCamPosition - In[0].vPosition).xyz;
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook));
    float3 vUp = normalize(cross(vLook, vRight));
    
    vRight *= In[0].vPSize.x * 0.5f * (fTime);
    vUp *= In[0].vPSize.y * 0.5f * (fTime);
    
    Out[0].vPosition = float4(In[0].vPosition.xyz + vRight + vUp, 1.f);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;
    Out[0].vColor = In[0].vColor;

    Out[1].vPosition = float4(In[0].vPosition.xyz - vRight + vUp, 1.f);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;
    Out[1].vColor = In[0].vColor;

    Out[2].vPosition = float4(In[0].vPosition.xyz - vRight - vUp, 1.f);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    Out[2].vColor = In[0].vColor;

    Out[3].vPosition = float4(In[0].vPosition.xyz + vRight - vUp, 1.f);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;
    Out[3].vColor = In[0].vColor;

    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);

    Container.Append(Out[0]);
    Container.Append(Out[1]);
    Container.Append(Out[2]);
    Container.RestartStrip(); // ���⼭���� �ٽ� ��ڴ�. �̰� �����ָ� 123���� ������.

    Container.Append(Out[0]);
    Container.Append(Out[2]);
    Container.Append(Out[3]);
    Container.RestartStrip();
}

[maxvertexcount(6)] // �� ����� ��. ���� �� �� ���� ����.(�ε��� ����)
void GS_FLOATYBITS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Container)
{
    GS_OUT Out[4];

	// �������.
    float3 vLook = (g_vCamPosition - In[0].vPosition).xyz;
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook));
    float3 vUp = normalize(cross(vLook, vRight));
    
    vRight *= In[0].vPSize.x * 0.5f;
    vUp *= In[0].vPSize.y * 3.f;
    
    Out[0].vPosition = float4(In[0].vPosition.xyz + vRight + vUp, 1.f);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;
    Out[0].vColor = In[0].vColor;

    Out[1].vPosition = float4(In[0].vPosition.xyz - vRight + vUp, 1.f);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;
    Out[1].vColor = In[0].vColor;

    Out[2].vPosition = float4(In[0].vPosition.xyz - vRight - vUp, 1.f);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    Out[2].vColor = In[0].vColor;

    Out[3].vPosition = float4(In[0].vPosition.xyz + vRight - vUp, 1.f);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;
    Out[3].vColor = In[0].vColor;

    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);

    Container.Append(Out[0]);
    Container.Append(Out[1]);
    Container.Append(Out[2]);
    Container.RestartStrip(); // ���⼭���� �ٽ� ��ڴ�. �̰� �����ָ� 123���� ������.

    Container.Append(Out[0]);
    Container.Append(Out[2]);
    Container.Append(Out[3]);
    Container.RestartStrip();
}


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : COLOR0;
    float4 vColor : COLOR1;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

/* 1. �ȼ��� �������� ������ �����Ѵ�. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    if (Out.vColor.a <= 0.3f)
        discard;

    Out.vColor.rgb = In.vColor.rgb;
    Out.vColor.a = Out.vColor.a * (1.f - (In.vLifeTime.y / In.vLifeTime.x)); //1~0

    if (In.vLifeTime.y >= In.vLifeTime.x)	// 1���� Ŀ���� ��.
        discard;

    return Out;
}

PS_OUT PS_LIGHT_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    if (Out.vColor.a <= 0.3f)
        discard;
	
    Out.vColor.r = In.vColor.r * (1.f - ((In.vLifeTime.y / In.vLifeTime.x) * 0.25f));
    Out.vColor.g = In.vColor.g * (1.f - ((In.vLifeTime.y / In.vLifeTime.x) * 0.25f));
    Out.vColor.b = In.vColor.b * (1.f - ((In.vLifeTime.y / In.vLifeTime.x) * 0.25f));

    return Out;
}

PS_OUT PS_LIGHTLONG_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
	
    Out.vColor.a = Out.vColor.r;

    if (Out.vColor.a <= 0.1f)
        discard;

    if (In.vLifeTime.y >= In.vLifeTime.x)	// 1���� Ŀ���� ��.
        discard;

    Out.vColor.r = 1.f - (1 - In.vColor.r) * (1 - Out.vColor.a);
    Out.vColor.g = 1.f - (1 - In.vColor.g) * (1 - Out.vColor.a);
    Out.vColor.b = 1.f - (1 - In.vColor.b) * (1 - Out.vColor.a);
        
	
    return Out;
}

PS_OUT PS_SPAWN_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
	
    Out.vColor.a = Out.vColor.r;

    if (Out.vColor.a <= 0.1f)
        discard;

    if (In.vLifeTime.y >= In.vLifeTime.x)	// 1���� Ŀ���� ��.
        discard;

    Out.vColor.rgb = In.vColor.rgb;
        

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

PS_OUT PS_SWORDPARTICLE_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float2 start = (float2) 0;
    float2 over = (float2) 0;
	
    int iTexIndex = (int) ((In.vLifeTime.y / In.vLifeTime.x) * (g_vTexDivide.x * g_vTexDivide.y - 1.f));
    
    start.x = (1 / g_vTexDivide.x) * iTexIndex;
    start.y = (1 / g_vTexDivide.y) * (int) (iTexIndex / g_vTexDivide.x);
	
    over.x = start.x + (1 / g_vTexDivide.x);
    over.y = start.y + (1 / g_vTexDivide.y);
	
    float2 vTexcoord = start + (over - start) * In.vTexcoord;
	
    
    if(In.vColor.r < 0.5f)
    {
        Out.vColor = g_Texture.Sample(LinearSampler, vTexcoord);
    }
    else
    {
        Out.vColor = g_SubTexture.Sample(LinearSampler, vTexcoord);
    }
    
    Out.vColor.a = Out.vColor.r;
    //Out.vColor.rgb *= 1.5f;
    Out.vColor.a *= 1.f - (In.vLifeTime.y / In.vLifeTime.x);
        
    if (Out.vColor.a <= 0.1f)
        discard;
    
    return Out;
}

PS_OUT PS_CONSTRUCT_BLACK_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float2 vTexcoord = (float2) 0;
    
    float fRadian = (In.vLifeTime.y / In.vLifeTime.x) * 3.141592f * In.vColor.g;
    
    vTexcoord.x = (In.vTexcoord.x - 0.5f) * cos(fRadian) - (In.vTexcoord.y - 0.5f) * sin(fRadian);
    vTexcoord.y = (In.vTexcoord.x - 0.5f) * sin(fRadian) + (In.vTexcoord.y - 0.5f) * cos(fRadian);
    
    vTexcoord.x += 0.5f;
    vTexcoord.y += 0.5f;
    
    Out.vColor = g_Texture.Sample(LinearSampler, vTexcoord);
    
    float fPer = (100.f * In.vColor.r) % 3.f;
    
    if(fPer == 0.f)
    {
        Out.vColor.a = Out.vColor.r;
    }
    else if(fPer == 1.f)
    {
        Out.vColor.a = Out.vColor.g;
    }
    else
    {
        Out.vColor.a = Out.vColor.b;
    }
    Out.vColor.a *= 1.f - (In.vLifeTime.y / In.vLifeTime.x);
    
    if (Out.vColor.a <= 0.1f)
        discard;
    
    Out.vColor.rgb = float3(0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_CONSTRUCT_PUPPLE_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float2 vTexcoord = (float2) 0;
    
    float2 start = (float2) 0;
    float2 over = (float2) 0;
	
    int iTexIndex = (int) ((In.vLifeTime.y / In.vLifeTime.x) * (g_vTexDivide.x * g_vTexDivide.y - 1.f));
    
    start.x = (1 / g_vTexDivide.x) * iTexIndex;
    start.y = (1 / g_vTexDivide.y) * (int) (iTexIndex / g_vTexDivide.x);
	
    over.x = start.x + (1 / g_vTexDivide.x);
    over.y = start.y + (1 / g_vTexDivide.y);
	
    vTexcoord = start + (over - start) * In.vTexcoord;
    
    Out.vColor = g_Texture.Sample(LinearSampler, vTexcoord);
    Out.vColor.a = Out.vColor.r;
    Out.vColor.rgb = Out.vColor.a * g_vColor.rgb;
    
    Out.vColor.a *= 1.f - (In.vLifeTime.y / In.vLifeTime.x);
    if (Out.vColor.a <= 0.1f)
        discard;
    
    
    return Out;
}

PS_OUT PS_SKILL_PUPPLE_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float2 vTexcoord = (float2) 0;
    
    float2 start = (float2) 0;
    float2 over = (float2) 0;
	
    int iTexIndex = (int) ((In.vLifeTime.y / In.vLifeTime.x) * (g_vTexDivide.x * g_vTexDivide.y - 1.f));
    
    start.x = (1 / g_vTexDivide.x) * iTexIndex;
    start.y = (1 / g_vTexDivide.y) * (int) (iTexIndex / g_vTexDivide.x);
	
    over.x = start.x + (1 / g_vTexDivide.x);
    over.y = start.y + (1 / g_vTexDivide.y);
	
    vTexcoord = start + (over - start) * In.vTexcoord;
    
    if (In.vColor.r < 0.5f)
    {
        Out.vColor = g_Texture.Sample(LinearSampler, vTexcoord);
    }
    else
    {
        Out.vColor = g_SubTexture.Sample(LinearSampler, vTexcoord);
    }
    
    Out.vColor.rgb = 0.f;
    
    Out.vColor.a *= 1.f - (In.vLifeTime.y / In.vLifeTime.x);
    if (Out.vColor.a <= 0.1f)
        discard;
    
    return Out;
}

PS_OUT PS_FLOATYBITS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float2 vTexcoord = (float2) 0;
    
    float2 start = (float2) 0;
    float2 over = (float2) 0;
	
    int iTexIndex = (int) (In.vColor.r * 100.f) % (int)(g_vTexDivide.x * g_vTexDivide.y);
    
    start.x = (1 / g_vTexDivide.x) * iTexIndex;
    start.y = (1 / g_vTexDivide.y) * (int) (iTexIndex / g_vTexDivide.x);
	
    over.x = start.x + (1 / g_vTexDivide.x);
    over.y = start.y + (1 / g_vTexDivide.y);
	
    vTexcoord = start + (over - start) * In.vTexcoord;
    
    Out.vColor = g_Texture.Sample(LinearSampler, vTexcoord);
    
    if (Out.vColor.a <= 0.1f)
        discard;
    
    float fTimeRatio = (In.vLifeTime.y / In.vLifeTime.x);
    
    if(fTimeRatio < 0.5f)
        Out.vColor.a *= fTimeRatio * 2.f;
    else
        Out.vColor.a *= (1.f - fTimeRatio) * 2.f;
    
    Out.vColor.rgb = 0.f;
    
    return Out;
}

technique11 DefaultTechnique
{
	/* ������ + ������Ʈ + ssao + ��ָ��� + pbr*/
    pass UI // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass LIGHT // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_LIGHT_MAIN();
    }

    pass LIGHTLONG // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_DIRECTION_MAIN();
        PixelShader = compile ps_5_0 PS_LIGHTLONG_MAIN();
    }

    pass SPAWN // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_DIRECTION_MAIN();
        PixelShader = compile ps_5_0 PS_SPAWN_MAIN();
    }

    pass STONE // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_STONE_MAIN();
    }

    pass WATERDROP // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_SPARK_MAIN();
        PixelShader = compile ps_5_0 PS_LIGHTLONG_MAIN();
    }

    pass SWORD // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_SWORDPARTICLE_MAIN();
        GeometryShader = compile gs_5_0 GS_SWORDPARTICLE_MAIN();
        PixelShader = compile ps_5_0 PS_SWORDPARTICLE_MAIN();
    }

    pass WAYPOINT // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_SPARK_DIR_MAIN();
        PixelShader = compile ps_5_0 PS_LIGHTLONG_MAIN();
    }

    pass SAVE // 8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_SWORDPARTICLE_MAIN();
        PixelShader = compile ps_5_0 PS_LIGHTLONG_MAIN();
    }

    pass CONSTRUCT_SWORD_BLACK // 9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_SWORDPARTICLE_MAIN();
        GeometryShader = compile gs_5_0 GS_SWORDPARTICLE_MAIN();
        PixelShader = compile ps_5_0 PS_CONSTRUCT_BLACK_MAIN();
    }

    pass CONSTRUCT_SWORD_PUPPLE // 10
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_SWORDPARTICLE_MAIN();
        GeometryShader = compile gs_5_0 GS_SWORDPARTICLE_MAIN();
        PixelShader = compile ps_5_0 PS_CONSTRUCT_PUPPLE_MAIN();
    }

    pass SKILL_PUPPLE // 11
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_SWORDPARTICLE_MAIN();
        GeometryShader = compile gs_5_0 GS_SWORDPARTICLE_MAIN();
        PixelShader = compile ps_5_0 PS_SKILL_PUPPLE_MAIN();
    }

    pass FLOATY_BITS // 12
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_SWORDPARTICLE_MAIN();
        GeometryShader = compile gs_5_0 GS_FLOATYBITS_MAIN();
        PixelShader = compile ps_5_0 PS_FLOATYBITS_MAIN();
    }
}