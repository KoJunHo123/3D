#include "stdafx.h"
#include "Lab_Construct_Attack.h"
#include "GameInstance.h"

#include "Player.h"
#include "Effect_BloodCore.h"

CLab_Construct_Attack::CLab_Construct_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CLab_Construct_Attack::CLab_Construct_Attack(const CLab_Construct_Attack& Prototype)
	: CPartObject(Prototype)
{
}

HRESULT CLab_Construct_Attack::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLab_Construct_Attack::Initialize(void* pArg)
{
	ATTACK_DESC* pDesc = static_cast<ATTACK_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketMatrix;
	m_fDamage = pDesc->fDamage;
	m_pAttackActive = pDesc->pAttackActive;
	m_vCenter = pDesc->vCenter;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc->vExtents, pDesc->vCenter, pDesc->vAngles)))
		return E_FAIL;

	return S_OK;
}

_uint CLab_Construct_Attack::Priority_Update(_float fTimeDelta)
{
	m_pColliderCom->Set_OnCollision(*m_pAttackActive);

	if (true == *m_pAttackActive)
	{
		if (false == m_bAttackSound)
		{
			SOUND_DESC  desc = {};
			desc.fMaxDistance = DEFAULT_DISTANCE;
			desc.fVolume = 1.f;
			XMStoreFloat3(&desc.vPos, XMLoadFloat4x4(m_pParentMatrix).r[3]);

			if(LAB_CONSTRUCT_ATK_3HIT_COMBO == m_eConstructAnim)
			{
				m_pGameInstance->SoundPlay_Additional(TEXT("Goblin_Caster_Fire_Whoosh_04.ogg"), desc);
			}
			else if(LAB_CONSTRUCT_ATK_DOWNSWIPE == m_eConstructAnim || LAB_CONSTRUCT_ATK_VERTICALSLAM == m_eConstructAnim)
			{
				m_pGameInstance->SoundPlay_Additional(TEXT("Foley_Construct_Heavy_Long_05.ogg"), desc);
			}
			else
			{
				m_pGameInstance->SoundPlay_Additional(TEXT("Foley_Construct_Heavy_Short_05.ogg"), desc);
			}

			m_bAttackSound = true;
		}
	}
	else
		m_bAttackSound = false;


	return OBJ_NOEVENT;
}

void CLab_Construct_Attack::Update(_float fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	// ??
	for (size_t i = 0; i < 3; i++)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}

	// �� ��ġ * �ٿ��� �� ���� ��ġ * �÷��̾� ��ġ -> �÷��̾��� ��ġ���� �ٿ��� �� ���� ��ġ.
	// -> ���̴����� ���ִ� �� * ���带 ���⼭ �ϴ� ��.
	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

	m_pColliderCom->Update(&m_WorldMatrix);
}

void CLab_Construct_Attack::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
#ifdef _DEBUG
	m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
}

HRESULT CLab_Construct_Attack::Render()
{
	return S_OK;
}

void CLab_Construct_Attack::Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval)
{
	if (true == *m_pAttackActive && TEXT("Coll_Player") == strColliderTag)
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(pCollisionObject);
		pPlayer->Be_Damaged(m_fDamage, XMLoadFloat4x4(m_pParentMatrix).r[3]);

		if (true == pPlayer->Be_Damaged(m_fDamage, XMLoadFloat4x4(m_pParentMatrix).r[3]))
		{
			_vector vCenter = XMLoadFloat3(&m_vCenter);
			vCenter = XMVector3TransformCoord(vCenter, XMLoadFloat4x4(&m_WorldMatrix));
			_vector vTargetPos = pCollisionObject->Get_Position();
			vTargetPos.m128_f32[1] = vCenter.m128_f32[1];
			_vector vDir = vTargetPos - vCenter;
			_vector vPos = vCenter + vDir * 0.5f;
			_vector vMoveDir = XMLoadFloat4x4(&m_WorldMatrix).r[3] - XMLoadFloat3(&m_vPrePosition);
			Add_AttackParticle(vPos, XMVector3Normalize(vMoveDir));

			SOUND_DESC SoundDesc = {};
			SoundDesc.fMaxDistance = DEFAULT_DISTANCE;
			SoundDesc.fVolume = 1.f;
			XMStoreFloat3(&SoundDesc.vPos, pCollisionObject->Get_Position());

			m_pGameInstance->SoundPlay_Additional(TEXT("SFX_Impacts_Root_Axe_Flesh_02.ogg"), SoundDesc);
		}


	}
}

HRESULT CLab_Construct_Attack::Ready_Components(_float3 vExtents, _float3 vCenter, _float3 vAngles)
{
	/* For.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderOBBDesc{};
	ColliderOBBDesc.vExtents = vExtents;
	ColliderOBBDesc.vCenter = vCenter;
	ColliderOBBDesc.vAngles = vAngles;

	CCollider::COLLIDER_DESC ColliderDesc = {};
	ColliderDesc.pOwnerObject = this;
	ColliderDesc.pBoundingDesc = &ColliderOBBDesc;
	ColliderDesc.bCollisionOnce = true;
	ColliderDesc.strColliderTag = TEXT("Coll_Monster_Attack");

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLab_Construct_Attack::Add_AttackParticle(_fvector vPos, _fvector vDir)
{
	CEffect_BloodCore::BLOOD_DESC desc = {};

	desc.fRotationPerSec = 0.f;
	desc.fSpeedPerSec = 1.f;
	XMStoreFloat3(&desc.vPos, vPos);
	XMStoreFloat3(&desc.vDir, vDir);
	desc.vScale = _float3(5.f, 5.f, 5.f);

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_BloodCore"), &desc)))
		return E_FAIL;

	return S_OK;
}

CLab_Construct_Attack* CLab_Construct_Attack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLab_Construct_Attack* pInstance = new CLab_Construct_Attack(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CLab_Construct_Attack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLab_Construct_Attack::Clone(void* pArg)
{
	CLab_Construct_Attack* pInstance = new CLab_Construct_Attack(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CLab_Construct_Attack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLab_Construct_Attack::Pooling()
{
	return new CLab_Construct_Attack(*this);
}

void CLab_Construct_Attack::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
