#include "..\Public\Picking.h"
#include "GameInstance.h"

CPicking::CPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{CGameInstance::Get_Instance()}
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CPicking::Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
	m_hWnd = hWnd;

	m_iWinSizeX = iWinSizeX;

	m_iWinSizeY = iWinSizeY;

	return S_OK;
}

void CPicking::Update()
{
	POINT			ptMouse{};

	GetCursorPos(&ptMouse);

	/* ����Ʈ == ���� * ������� * ����� * ������� /w -> ����Ʈ ��ȯ */
	ScreenToClient(m_hWnd, &ptMouse);

	/* ���������̽� == ���� * ������� * ����� * ������� /w */
	_float4		vMousePos{};
	vMousePos.x = ptMouse.x / (m_iWinSizeX * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / (m_iWinSizeY * -0.5f) + 1.f;
	vMousePos.z = 0.f; /* Near����� Ŭ���Ѱž�!! */
	vMousePos.w = 1.f;

	_vector vMouseVec = XMLoadFloat4(&vMousePos);

	/* �佺���̽� == ���� * ������� * ����� */
	_matrix			ProjMatrixInv = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_PROJ);
	vMouseVec = XMVector3TransformCoord(vMouseVec, ProjMatrixInv);

	/* �佺���̽� �󿡼��� ���콺 ���̿� ������ �������� ���س��´�. */
	/* �佺���̽� ���̱� ������ ī�޶� ��ǥ�� �����̴�. */
	m_vRayPos = _float4(0.f, 0.f, 0.f, 1.f);
	XMStoreFloat4(&m_vRayDir, vMouseVec - XMLoadFloat4(&m_vRayPos));

	/* ���彺���̽� == ���� * ������� */
	_matrix			ViewMatrixInv = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW);

	XMStoreFloat4(&m_vRayPos, XMVector3TransformCoord(XMLoadFloat4(&m_vRayPos), ViewMatrixInv));
	XMStoreFloat4(&m_vRayDir, XMVector3TransformNormal(XMLoadFloat4(&m_vRayDir), ViewMatrixInv));

	XMStoreFloat4(&m_vRayDir, XMVector4Normalize(XMLoadFloat4(&m_vRayDir)));
}

void CPicking::Transform_ToLocalSpace(const _matrix& WorldMatrix)
{
	_matrix		WorldMatrixInverse = XMMatrixInverse(nullptr, WorldMatrix);

	XMStoreFloat4(&m_vRayPos_InLocalSpace, XMVector3TransformCoord(XMLoadFloat4(&m_vRayPos), WorldMatrixInverse));
	XMStoreFloat4(&m_vRayDir_InLocalSpace, XMVector3TransformNormal(XMLoadFloat4(&m_vRayDir), WorldMatrixInverse));
	
	XMStoreFloat4(&m_vRayDir_InLocalSpace, XMVector4Normalize(XMLoadFloat4(&m_vRayDir_InLocalSpace)));
}

_bool CPicking::isPicked_InWorldSpace(const _fvector& vPointA, const _fvector& vPointB, const _fvector& vPointC, _vector* pOut)
{
	_float		fU{}, fV{}, fDist{};

	_vector vOrigin = XMLoadFloat4(&m_vRayPos);
	_vector vDirection = XMLoadFloat4(&m_vRayDir);

	if (TRUE == TriangleTests::Intersects(vOrigin, vDirection, vPointA, vPointB, vPointC, fDist))
	{
		*pOut = vOrigin + vDirection * fDist;
		return true;
	}
	return false;
}

_bool CPicking::isPicked_InLocalSpace(const _fvector& vPointA, const _fvector& vPointB, const _fvector& vPointC, _vector* pOut)
{
	_float		fU{}, fV{}, fDist{};

	_vector vOrigin = XMLoadFloat4(&m_vRayPos_InLocalSpace);
	_vector vDirection = XMLoadFloat4(&m_vRayDir_InLocalSpace);

	if (TRUE == TriangleTests::Intersects(vOrigin, vDirection, vPointA, vPointB, vPointC, fDist))
	{
		*pOut = vOrigin + vDirection * fDist;
		return true;
	}
	return false;
}

_bool CPicking::isPicked_InLocalSpace(const _fvector& vPointA, const _fvector& vPointB, const _fvector& vPointC, _vector* pOut, _float* pDist)
{
	_float		fU{}, fV{}, fDist{};

	_vector vOrigin = XMLoadFloat4(&m_vRayPos_InLocalSpace);
	_vector vDirection = XMLoadFloat4(&m_vRayDir_InLocalSpace);

	if (TRUE == TriangleTests::Intersects(vOrigin, vDirection, vPointA, vPointB, vPointC, fDist))
	{
		if(*pDist > fDist)
		{
			*pDist = fDist;
			*pOut = vOrigin + vDirection * fDist;
			return true;
		}
	}
	return false;
}

CPicking* CPicking::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
	CPicking* pInstance = new CPicking(pDevice, pContext);

	if (FAILED(pInstance->Initialize(hWnd, iWinSizeX, iWinSizeY)))
	{
		MSG_BOX(TEXT("Failed to Created : CPicking"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPicking::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}
