#include "stdafx.h"
#include "Particle_DragonHeart.h"
#include "GameInstance.h"

CParticle_DragonHeart::CParticle_DragonHeart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CParticle_DragonHeart::CParticle_DragonHeart(const CParticle_DragonHeart& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CParticle_DragonHeart::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle_DragonHeart::Initialize(void* pArg)
{
	PARTICLE_DRAGONHEART_DESC* pDesc = static_cast<PARTICLE_DRAGONHEART_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Set_Position(XMLoadFloat3(&pDesc->vPos));

	return S_OK;
}

_uint CParticle_DragonHeart::Priority_Update(_float fTimeDelta)
{
	if (true == m_bDead)
		return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CParticle_DragonHeart::Update(_float fTimeDelta)
{
	if(true == m_pVIBufferCom->Spread(XMVectorSet(0.f, 0.f, 0.f, 1.f), 1.f, 0.f, false, fTimeDelta))
		m_bDead = true;
}

void CParticle_DragonHeart::Late_Update(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLOOM, this);
}

HRESULT CParticle_DragonHeart::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Begin(5)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_DragonHeart::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle_Spark"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_DragonHeart"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CParticle_DragonHeart* CParticle_DragonHeart::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticle_DragonHeart* pInstance = new CParticle_DragonHeart(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CParticle_DragonHeart"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CParticle_DragonHeart::Clone(void* pArg)
{
	CParticle_DragonHeart* pInstance = new CParticle_DragonHeart(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CParticle_DragonHeart"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CParticle_DragonHeart::Pooling()
{
	return new CParticle_DragonHeart(*this);
}

void CParticle_DragonHeart::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
