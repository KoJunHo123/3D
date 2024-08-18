#include "stdafx.h"
#include "Player_State.h"
#include "GameInstance.h"

CPlayer_State::CPlayer_State()
{
}

HRESULT CPlayer_State::Initialize(void* pArg)
{
    PLAYER_STATE_DESC* pDesc = static_cast<PLAYER_STATE_DESC*>(pArg);

    __super::Initialize(pArg);

    m_pTransformCom = pDesc->pTransformCom;
    Safe_AddRef(m_pTransformCom);

    m_pShaderCom = pDesc->pShaderCom;
    Safe_AddRef(m_pShaderCom);

    m_pModelCom = pDesc->pModelCom;
    Safe_AddRef(m_pModelCom);

    m_pSpeed = pDesc->pSpeed;

    return S_OK;
}

HRESULT CPlayer_State::StartState(void** pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    m_AnimDesc = *static_cast<PLAYER_ANIM_DESC*>(*pArg);

    return S_OK;
}

void CPlayer_State::Priority_Update(_float fTimeDelta)
{
    if (true == m_isChanged)
    {
        m_AnimDesc.eCurrentPlayerAnim = m_AnimDesc.eNextPlayerAnim;
        m_isChanged = false;
    }
}

void CPlayer_State::Update(_float fTimeDelta)
{

}

void CPlayer_State::Late_Update(_float fTimeDelta)
{
}

HRESULT CPlayer_State::Render()
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
        if (FAILED(m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
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

HRESULT CPlayer_State::ExitState(void** pArg)
{
    *pArg = &m_AnimDesc;
    m_isFinished = false;

    return S_OK;
}


_vector CPlayer_State::Get_Rotation(_matrix WorldMatrix, _vector vExist)
{
    _vector vScale, vRotationQuat, vTranslation;
    XMMatrixDecompose(&vScale, &vRotationQuat, &vTranslation, WorldMatrix);

    // 1�� ���
    // ȸ�� ��� ���� (���ʹϾ��� ȸ�� ��ķ� ��ȯ)
    XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(vRotationQuat);
    // ȸ�� ����� ���� v�� ����
    // �� -1 ���ؾ� �ϴ����� �𸣰���; �ƴ� 180�� �������ϱ� ȸ���� �Ųٷ� �Կ��� �ϳ� ��.
    return -1 * XMVector3TransformNormal(vExist, rotationMatrix);

#pragma region ���ʹϾ� ȸ��
    // 2�� ��� : �̰� ���� �̻��ϰ� ��.
    //// ���ʹϾ��� ���� ���
    //_vector qInverse = XMQuaternionInverse(vRotationQuat);

    //// v�� ���ʹϾ����� ��ȯ (w=0, x, y, z = ���� ��)
    //_vector vQuat = XMVectorSet(XMVectorGetX(vExist), XMVectorGetY(vExist), XMVectorGetZ(vExist), 0.0f);

    //// ȸ�� ����: v' = q * v * q^-1
    //_vector rotatedVQuat = XMQuaternionMultiply(XMQuaternionMultiply(vRotationQuat, vQuat), qInverse);

    //// ȸ���� ���� ���� (x, y, z)
    //return XMVectorSet(XMVectorGetX(rotatedVQuat), XMVectorGetY(rotatedVQuat), XMVectorGetZ(rotatedVQuat), 0.0f);
#pragma endregion �ٵ� �� �ȵ�
}

void CPlayer_State::Play_Animation(_float fTimeDelta)
{
    _vector vStateChange{};
    m_isFinished = m_pModelCom->Play_Animation(fTimeDelta, vStateChange);
    vStateChange = Get_Rotation(m_pTransformCom->Get_WorldMatrix(), vStateChange);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vStateChange);
}

void CPlayer_State::Change_Animation(_float fTimeDelta)
{
    _vector vStateChange{};
    m_isChanged = m_pModelCom->Change_Animation(fTimeDelta, vStateChange, m_fChangeRate);
    vStateChange = Get_Rotation(m_pTransformCom->Get_WorldMatrix(), vStateChange);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vStateChange);
}

void CPlayer_State::SetUp_Animation(ANIM_PLAYER eNextPlayerAnim, _bool isLoop)
{
    _bool isChanging = m_AnimDesc.eNextPlayerAnim != eNextPlayerAnim;
    if (true == IsChanging() && true == isChanging)
         int a = 0;

    if (true == IsChanging())
        return;

    m_AnimDesc.eNextPlayerAnim = eNextPlayerAnim;
    m_pModelCom->SetUp_Animation(eNextPlayerAnim, isLoop);
}

void CPlayer_State::Look_CameraDir()
{
    CTransform* pCameraTransform = static_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), g_strTransformTag));

    _vector vCameraLook = pCameraTransform->Get_State(CTransform::STATE_LOOK);
    m_pTransformCom->LookDir(vCameraLook);

}

void CPlayer_State::Free()
{
    __super::Free();

    Safe_Release(m_pTransformCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}
