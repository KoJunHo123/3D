#include "stdafx.h"
#include "..\Public\Monster.h"
#include "GameInstance.h"

CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CMonster::CMonster(const CMonster& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	MONSTER_DESC*		Desc = static_cast<MONSTER_DESC*>(pArg);

	/* ���������� ���� �����͵��� ��� ���ϳ�. */
	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Desc->vPos);
	m_pTransformCom->Set_Scaled(Desc->vScale.x, Desc->vScale.y, Desc->vScale.z);
	m_pTransformCom->Rotation(Desc->vRotationAxis, XMConvertToRadians(Desc->fRotationAngle));

	if (FAILED(Ready_Components()))
		return E_FAIL;
	//m_iCurrentAnimationIndex = (_uint)m_pGameInstance->Get_Random(0.f, 20.f);
	m_iCurrentAnimationIndex = 34;
	m_pModelCom->SetUp_Animation(m_iCurrentAnimationIndex, false);

	return S_OK;
}

void CMonster::Priority_Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(VK_RIGHT))
		m_pModelCom->SetUp_Animation(++m_iCurrentAnimationIndex, false);

	if (m_pGameInstance->Key_Down(VK_LEFT))
	{
		if(0 < m_iCurrentAnimationIndex)
			m_pModelCom->SetUp_Animation(--m_iCurrentAnimationIndex, false);
	}
}

void CMonster::Update(_float fTimeDelta)
{
	_vector vStateChange{};
	m_pModelCom->Play_Animation(fTimeDelta, vStateChange);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vStateChange);
}

void CMonster::Late_Update(_float fTimeDelta)
{
	/* ���������� ���� ������ı��� �����ϰ� �ȴ�. */
	__super::Late_Update(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CMonster::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bine_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonster::Save_Data(ofstream* pOutFile)
{
	pOutFile->write(reinterpret_cast<const _char*>(&m_pTransformCom->Get_WorldMatrix()), sizeof(_float4x4));

	return S_OK;
}

HRESULT CMonster::Load_Data(ifstream* pInFile)
{
	_float4x4 WorldMatrix = {};
	pInFile->read(reinterpret_cast<_char*>(&WorldMatrix), sizeof(_float4x4));
	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&WorldMatrix));

	return S_OK;
}

HRESULT CMonster::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CMonster* CMonster::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster* pInstance = new CMonster(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonster::Clone(void* pArg)
{
	CMonster* pInstance = new CMonster(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);

	Safe_Release(m_pModelCom);
}
