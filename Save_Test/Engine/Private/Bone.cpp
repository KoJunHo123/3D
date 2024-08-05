#include "Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(const aiNode* pAIBone, _int iParentBoneIndex)
{
    m_iParentBoneIndex = iParentBoneIndex;
    // �� �̸� ����
    strcpy_s(m_szName, pAIBone->mName.data);
    // �ڷ����� �޶� ���°� �Ȱ����ϱ� �޸� �״�� ����
    memcpy(&m_TransformationMatrix, &pAIBone->mTransformation, sizeof(_float4x4));
    // �̰� Assimp���� �������� ����� RUL�� �� �������� �Ǿ� �־ ��ġ�� ���� ��������� �ٲ���
    XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
    // CombinedTransformation -> �̰� �θ� ��� �޾Ƽ� ����� ��������ε� �� �����Ӹ��� �ٲ���� �Ǵ� �Ŷ� �ϴ� �׵����� �ʱ�ȭ.
    XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

    return S_OK;
}

void CBone::Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix)
{
    if (-1 == m_iParentBoneIndex)
        XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * PreTransformMatrix);
    else
        XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * Bones[m_iParentBoneIndex]->Get_CombinedTransformationMatrix());
}

void CBone::Save_Data(ofstream* outfile)
{
    outfile->write(reinterpret_cast<const char*>(m_szName), sizeof(_char) * MAX_PATH);
    outfile->write(reinterpret_cast<const char*>(&m_TransformationMatrix), sizeof(_float4x4));
    outfile->write(reinterpret_cast<const char*>(&m_iParentBoneIndex), sizeof(_int));
}

CBone* CBone::Create(const aiNode* pAIBone, _int iParentBoneIndex)
{
    CBone* pInstance = new CBone;

    if (FAILED(pInstance->Initialize(pAIBone, iParentBoneIndex)))
    {
        MSG_BOX(TEXT("Failed to Created : CBone"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBone::Free()
{
    __super::Free();
}
