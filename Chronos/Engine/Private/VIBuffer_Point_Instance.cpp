#include "..\Public\VIBuffer_Point_Instance.h"

#include "GameInstance.h"

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instancing{ pDevice, pContext }
{
}

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& Prototype)
	: CVIBuffer_Instancing{ Prototype }
{
}

HRESULT CVIBuffer_Point_Instance::Initialize_Prototype(const CVIBuffer_Instancing::INSTANCE_DESC& Desc)
{
	if (FAILED(__super::Initialize_Prototype(Desc)))
		return E_FAIL;

	m_iNumVertexBuffers = 2;
	m_iNumVertices = 1;
	m_iVertexStride = sizeof(VTXPOINT);
	m_iNumIndices = m_iNumInstance;
	m_iIndexStride = 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	m_iInstanceStride = sizeof(VTXPOINTINSTANCE);
	m_iIndexCountPerInstance = 1;

#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* 정점버퍼에 채워줄 값들을 만들기위해서 임시적으로 공간을 할당한다. */
	VTXPOINT* pVertices = new VTXPOINT[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOINT) * m_iNumVertices);

	_float	fScale = m_pGameInstance->Get_Random(m_vSize.x, m_vSize.y);

	pVertices->vPosition = _float3(0.f, 0.f, 0.f);
	pVertices->vPSize = _float2(fScale, fScale);


	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	/* 정점버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	/* 인덱스버퍼의 내용을 채워주곡 */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iIndexStride;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	/* 인덱스버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

#pragma region INSTANCE_BUFFER

	ZeroMemory(&m_InstanceBufferDesc, sizeof m_InstanceBufferDesc);
	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC; /* 정적버퍼로 생성한다. */
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;

	m_pInstanceVertices = new VTXPOINTINSTANCE[m_iNumInstance];
	ZeroMemory(m_pInstanceVertices, sizeof(VTXPOINTINSTANCE) * m_iNumInstance);

	VTXPOINTINSTANCE* pInstanceVertices = static_cast<VTXPOINTINSTANCE*>(m_pInstanceVertices);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pInstanceVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);

		_float fX_Small{}, fX_Big{}, fY_Small{}, fY_Big{}, fZ_Small{}, fZ_Big{};

		_float fX{}, fY{}, fZ{};

		while (true)
		{
			_float fRandomNum = m_pGameInstance->Get_Random_Normal();
			if (fRandomNum < 0.333f)
			{
				if (m_vRange.x == m_vExceptRange.x)
					continue;

				fX_Small = m_pGameInstance->Get_Random(m_vCenterPos.x - m_vRange.x * 0.5f, m_vCenterPos.x - m_vExceptRange.x * 0.5f);
				fX_Big = m_pGameInstance->Get_Random(m_vCenterPos.x + m_vExceptRange.x * 0.5f, m_vCenterPos.x + m_vRange.x * 0.5f);

				if (0.5f < m_pGameInstance->Get_Random_Normal())
					fX = fX_Small;
				else
					fX = fX_Big;
				fY = m_pGameInstance->Get_Random(m_vCenterPos.y - m_vRange.y * 0.5f, m_vCenterPos.y + m_vRange.y * 0.5f);
				fZ = m_pGameInstance->Get_Random(m_vCenterPos.z - m_vRange.z * 0.5f, m_vCenterPos.z + m_vRange.z * 0.5f);
			}
			else if (0.333f <= fRandomNum && fRandomNum < 0.666f)
			{
				if (m_vRange.y == m_vExceptRange.y)
					continue;

				fY_Small = m_pGameInstance->Get_Random(m_vCenterPos.y - m_vRange.y * 0.5f, m_vCenterPos.y - m_vExceptRange.y * 0.5f);
				fY_Big = m_pGameInstance->Get_Random(m_vCenterPos.y + m_vExceptRange.y * 0.5f, m_vCenterPos.y + m_vRange.y * 0.5f);

				if (0.5f < m_pGameInstance->Get_Random_Normal())
					fY = fY_Small;
				else
					fY = fY_Big;

				fX = m_pGameInstance->Get_Random(m_vCenterPos.x - m_vRange.x * 0.5f, m_vCenterPos.x + m_vRange.x * 0.5f);
				fZ = m_pGameInstance->Get_Random(m_vCenterPos.z - m_vRange.z * 0.5f, m_vCenterPos.z + m_vRange.z * 0.5f);
			}
			else
			{
				if (m_vRange.z == m_vExceptRange.z)
					continue;

				fZ_Small = m_pGameInstance->Get_Random(m_vCenterPos.z - m_vRange.z * 0.5f, m_vCenterPos.z - m_vExceptRange.z * 0.5f);
				fZ_Big = m_pGameInstance->Get_Random(m_vCenterPos.z + m_vExceptRange.z * 0.5f, m_vCenterPos.z + m_vRange.z * 0.5f);

				if (0.5f < m_pGameInstance->Get_Random_Normal())
					fZ = fZ_Small;
				else
					fZ = fZ_Big;

				fX = m_pGameInstance->Get_Random(m_vCenterPos.x - m_vRange.x * 0.5f, m_vCenterPos.x + m_vRange.x * 0.5f);
				fY = m_pGameInstance->Get_Random(m_vCenterPos.y - m_vRange.y * 0.5f, m_vCenterPos.y + m_vRange.y * 0.5f);
			}

			break;
		}

		pInstanceVertices[i].vTranslation = _float4(fX, fY, fZ, 1.f);
		pInstanceVertices[i].vLifeTime = _float2(m_pGameInstance->Get_Random(m_vLifeTime.x, m_vLifeTime.y), 0.0f);
		pInstanceVertices[i].vColor =
			_float4(m_pGameInstance->Get_Random(m_vMinColor.x, m_vMaxColor.x),
				m_pGameInstance->Get_Random(m_vMinColor.y, m_vMaxColor.y),
				m_pGameInstance->Get_Random(m_vMinColor.z, m_vMaxColor.z),
				m_pGameInstance->Get_Random(m_vMinColor.w, m_vMaxColor.w));
	}

	ZeroMemory(&m_InstanceInitialData, sizeof m_InstanceInitialData);
	m_InstanceInitialData.pSysMem = m_pInstanceVertices;

#pragma endregion



	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::Initialize(void* pArg)
{
#pragma region INSTANCE_BUFFER
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
#pragma endregion

	m_CurrentRandomDir.resize(m_iNumInstance);
	m_NextRandomDir.resize(m_iNumInstance);
	Set_NextRandomDir();


	return S_OK;
}

_bool CVIBuffer_Point_Instance::Spread(_fvector vPivot, _float fSpeed, _float fGravity, _bool isLoop, _float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOINTINSTANCE* pVertices = static_cast<VTXPOINTINSTANCE*>(SubResource.pData);

	_bool isOver = { true };

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_vector		vDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - vPivot, 0.f);
		_vector vMoveDir = XMVector3Normalize(vDir) * m_pSpeed[i] * fSpeed;
		vMoveDir = XMVectorSetY(vMoveDir, XMVectorGetY(vMoveDir) - fGravity * pVertices[i].vLifeTime.y);
		pVertices[i].vLifeTime.y += fTimeDelta;

		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + vMoveDir * fTimeDelta);

		XMStoreFloat4(&pVertices[i].vLook, XMVector3Normalize(vMoveDir));

		if (true == isLoop && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			pVertices[i] = static_cast<VTXPOINTINSTANCE*>(m_pInstanceVertices)[i];
		}
		else if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
			isOver = false;
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	return isOver;
}


_bool CVIBuffer_Point_Instance::Move_Dir(_fvector vDir, _float fSpeed, _float fGravity, _bool isLoop, _float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	// 덮어쓰기 x
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOINTINSTANCE* pVertices = static_cast<VTXPOINTINSTANCE*>(SubResource.pData);
	
	_bool isOver = { true };
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_vector vMoveDir = XMVector3Normalize(vDir) * m_pSpeed[i] * fSpeed;
		vMoveDir = XMVectorSetY(vMoveDir, XMVectorGetY(vMoveDir) - fGravity * pVertices[i].vLifeTime.y);
		pVertices[i].vLifeTime.y += fTimeDelta;

		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + vMoveDir * fTimeDelta);

		XMStoreFloat4(&pVertices[i].vLook, XMVector3Normalize(vMoveDir));

		if (true == isLoop && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			pVertices[i] = static_cast<VTXPOINTINSTANCE*>(m_pInstanceVertices)[i];
		}
		else if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
			isOver = false;
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	return isOver;
}

_bool CVIBuffer_Point_Instance::Converge(_fvector vPivot, _float fSpeed, _bool isLoop, _float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOINTINSTANCE* pVertices = static_cast<VTXPOINTINSTANCE*>(SubResource.pData);

	_bool isOver = { true };
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_vector		vDir = XMVectorSetW(vPivot - XMLoadFloat4(&pVertices[i].vTranslation), 0.f);

		XMStoreFloat4(&pVertices[i].vTranslation,
			XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeed[i] * fSpeed * fTimeDelta);

		_float fLength = XMVectorGetX(XMVector3Length(vDir));
		_float fMoveLength = XMVectorGetX(XMVector3Length(XMVector3Normalize(vDir) * m_pSpeed[i] * fSpeed * fTimeDelta));
		XMStoreFloat4(&pVertices[i].vLook, XMVector3Normalize(vDir));

		pVertices[i].vLifeTime.y += fTimeDelta;
		if (fLength <= fMoveLength)
		{
			XMStoreFloat4(&pVertices[i].vTranslation, vPivot);
			if (true == isLoop)
			{
				pVertices[i] = static_cast<VTXPOINTINSTANCE*>(m_pInstanceVertices)[i];
			}
		}
		else
		{
			isOver = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	return isOver;
}

_bool CVIBuffer_Point_Instance::Revolve(_fvector vPivot, _float3 vRevolveRadian, _fvector vMoveDir, _float fSpeed, _bool isLoop, _float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOINTINSTANCE* pVertices = static_cast<VTXPOINTINSTANCE*>(SubResource.pData);

	_bool isOver = { true };

	for (size_t i = 0; i < m_iNumInstance; ++i)
	{
		_float fResultSpeed = m_pSpeed[i] * fSpeed * fTimeDelta;

		_vector		vDir = vPivot - XMLoadFloat4(&pVertices[i].vTranslation);
		_float		fLength = XMVectorGetX(XMVector3Length(vDir));
		_vector		vRotation = XMQuaternionRotationRollPitchYaw(vRevolveRadian.x * fResultSpeed, vRevolveRadian.y * fResultSpeed, vRevolveRadian.z * fResultSpeed);
		_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);
		_vector		vRotate = XMVector3TransformNormal(vDir, RotationMatrix);
		
		_vector vAdditionalMove = vMoveDir * fResultSpeed;
		_vector vResultDir = vDir - vRotate + vAdditionalMove;

		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + vResultDir);
		XMStoreFloat4(&pVertices[i].vLook, XMVector3Normalize(vResultDir));

		pVertices[i].vLifeTime.y += fTimeDelta;
		if (true == isLoop && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			pVertices[i] = static_cast<VTXPOINTINSTANCE*>(m_pInstanceVertices)[i];
		}
		else if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
			isOver = false;
	}

	m_pContext->Unmap(m_pVBInstance, 0);
	
	return isOver;
}

_bool CVIBuffer_Point_Instance::Spread_Random(_fvector vPivot, _float fSpeed, _float fGravity, _bool isLoop, _float fTimeInterval, _float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOINTINSTANCE* pVertices = static_cast<VTXPOINTINSTANCE*>(SubResource.pData);

	_bool isOver = { true };

	m_fTime += fTimeDelta;

	if (m_fTime > fTimeInterval)
	{
		m_fTime = 0.f;
		Set_NextRandomDir();
	}

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_vector	vDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - vPivot, 0.f);
		_vector vNormDir = XMVector3Normalize(XMVector3Normalize(vDir) + XMVectorLerp(XMLoadFloat3(&m_CurrentRandomDir[i]), XMLoadFloat3(&m_NextRandomDir[i]), m_fTime * (1.f / fTimeInterval)));
		_vector vMoveDir = vNormDir * m_pSpeed[i] * fSpeed;
		vMoveDir = XMVectorSetY(vMoveDir, XMVectorGetY(vMoveDir) - fGravity * pVertices[i].vLifeTime.y);
		pVertices[i].vLifeTime.y += fTimeDelta;

		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + vMoveDir * fTimeDelta);

		XMStoreFloat4(&pVertices[i].vLook, XMVector3Normalize(vMoveDir));

		if (true == isLoop && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			pVertices[i] = static_cast<VTXPOINTINSTANCE*>(m_pInstanceVertices)[i];
		}
		else if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
			isOver = false;
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	return isOver;
}

void CVIBuffer_Point_Instance::Reset()
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOINTINSTANCE* pVertices = static_cast<VTXPOINTINSTANCE*>(SubResource.pData);
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i] = static_cast<VTXPOINTINSTANCE*>(m_pInstanceVertices)[i];
	}
	m_bFirst = false;
	m_pContext->Unmap(m_pVBInstance, 0);
}

_bool CVIBuffer_Point_Instance::Trail_Points(_fmatrix WorldMatrix, _fvector vDir, _bool isLoop, _float fTimeDelta)
{
	if (false == m_bFirst)
	{
		XMStoreFloat4x4(&m_PreMatrix, WorldMatrix);
	}
	_matrix PreMatrix = XMLoadFloat4x4(&m_PreMatrix);

	_vector vCurrentCenter = XMVector3TransformCoord(XMLoadFloat3(&m_vCenterPos), WorldMatrix);
	_vector vPreCenter = XMVector3TransformCoord(XMLoadFloat3(&m_vCenterPos), XMLoadFloat4x4(&m_PreMatrix));
	_vector vDiff = vCurrentCenter - vPreCenter;

	_float fSpeed = XMVectorGetX(XMVector3Length(vDiff)) * 5.f;
	if (fSpeed < 1.f)
		fSpeed = 1.f;

	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOINTINSTANCE* pVertices = static_cast<VTXPOINTINSTANCE*>(SubResource.pData);
	_bool bOver = { true };

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		if (false == m_bFirst)
		{
			XMStoreFloat4(&pVertices[i].vTranslation, XMVector3TransformCoord(XMLoadFloat4(&pVertices[i].vTranslation), WorldMatrix));
		}

		_vector vMoveDir = XMVector3Normalize(vDir) * m_pSpeed[i];
		pVertices[i].vLifeTime.y += fTimeDelta * fSpeed;

		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + vMoveDir * fTimeDelta);
		XMStoreFloat4(&pVertices[i].vLook, XMVector3Normalize(vDir));

		if (true == isLoop && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			_vector vTranslation = {};
			_float fX = m_pGameInstance->Get_Random(XMVectorGetX(PreMatrix.r[3]), XMVectorGetX(PreMatrix.r[3] + vDiff));
			_float fY = m_pGameInstance->Get_Random(XMVectorGetY(PreMatrix.r[3]), XMVectorGetY(PreMatrix.r[3] + vDiff));
			_float fZ = m_pGameInstance->Get_Random(XMVectorGetZ(PreMatrix.r[3]), XMVectorGetZ(PreMatrix.r[3] + vDiff));

			vTranslation = XMVectorSet(fX, fY, fZ, 1.f);
			_matrix ChangedMatrix = WorldMatrix;
			ChangedMatrix.r[3] = vTranslation;

			XMStoreFloat4(&pVertices[i].vTranslation, XMVector3TransformCoord(XMLoadFloat4(&static_cast<VTXPOINTINSTANCE*>(m_pInstanceVertices)[i].vTranslation), ChangedMatrix));

			pVertices[i].vLifeTime.y = 0.f;
		}
		else if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
			bOver = false;

	}

	m_pContext->Unmap(m_pVBInstance, 0);

	XMStoreFloat4x4(&m_PreMatrix, WorldMatrix);

	if (false == m_bFirst)
		m_bFirst = true;

	return bOver;
}

_bool CVIBuffer_Point_Instance::Trail_Spread(_fmatrix WorldMatrix, _fvector vPivot, _float fGravity, _bool isLoop, _float fTimeDelta)
{
	if (false == m_bFirst)
	{
		XMStoreFloat4x4(&m_PreMatrix, WorldMatrix);
	}

	_matrix PreMatrix = XMLoadFloat4x4(&m_PreMatrix);
	
	_vector vCurrentCenter = XMVector3TransformCoord(XMLoadFloat3(&m_vCenterPos), WorldMatrix);
	_vector vPreCenter = XMVector3TransformCoord(XMLoadFloat3(&m_vCenterPos), XMLoadFloat4x4(&m_PreMatrix));
	_vector vDiff = vCurrentCenter - vPreCenter;

	_float fSpeed = XMVectorGetX(XMVector3Length(vDiff)) * 5.f;
	if (fSpeed < 1.f)
		fSpeed = 1.f;

	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOINTINSTANCE* pVertices = static_cast<VTXPOINTINSTANCE*>(SubResource.pData);

	_bool bOver = { true };
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		if (false == m_bFirst)
		{
			XMStoreFloat4(&pVertices[i].vTranslation, XMVector3TransformCoord(XMLoadFloat4(&pVertices[i].vTranslation), WorldMatrix));
		}

		_vector vWorldPivot = XMVector3TransformCoord(vPivot, WorldMatrix);
		_vector vDir = XMLoadFloat4(&pVertices[i].vTranslation) - vWorldPivot;

		_vector vMoveDir = XMVector3Normalize(vDir) * m_pSpeed[i];
		vMoveDir = XMVectorSetY(vMoveDir, XMVectorGetY(vMoveDir) - fGravity * pVertices[i].vLifeTime.y);
		pVertices[i].vLifeTime.y += fTimeDelta * fSpeed;

		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + vMoveDir * fTimeDelta);
		XMStoreFloat4(&pVertices[i].vLook, XMVector3Normalize(vDir));

		if (true == isLoop && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			_vector vTranslation = {};
			_float fX = m_pGameInstance->Get_Random(XMVectorGetX(PreMatrix.r[3]), XMVectorGetX(PreMatrix.r[3] + vDiff));
			_float fY = m_pGameInstance->Get_Random(XMVectorGetY(PreMatrix.r[3]), XMVectorGetY(PreMatrix.r[3] + vDiff));
			_float fZ = m_pGameInstance->Get_Random(XMVectorGetZ(PreMatrix.r[3]), XMVectorGetZ(PreMatrix.r[3] + vDiff));

			vTranslation = XMVectorSet(fX, fY, fZ, 1.f);
			_matrix ChangedMatrix = WorldMatrix;
			ChangedMatrix.r[3] = vTranslation;


			XMStoreFloat4(&pVertices[i].vTranslation, XMVector3TransformCoord(XMLoadFloat4(&static_cast<VTXPOINTINSTANCE*>(m_pInstanceVertices)[i].vTranslation), ChangedMatrix));

			pVertices[i].vLifeTime.y = 0.f;
		}
		else if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
			bOver = false;
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	XMStoreFloat4x4(&m_PreMatrix, WorldMatrix);

	if (false == m_bFirst)
		m_bFirst = true;

	return bOver;
}


void CVIBuffer_Point_Instance::Set_NextRandomDir()
{
	for (size_t i = 0; i < m_iNumInstance; ++i)
	{
		m_CurrentRandomDir[i] = m_NextRandomDir[i];

		m_NextRandomDir[i].x = m_pGameInstance->Get_Random(-1.f, 1.f);
		m_NextRandomDir[i].y = m_pGameInstance->Get_Random(-1.f, 1.f);
		m_NextRandomDir[i].z = m_pGameInstance->Get_Random(-1.f, 1.f);

		XMStoreFloat3(&m_NextRandomDir[i], XMVector3Normalize(XMLoadFloat3(&m_NextRandomDir[i])));
	}
}

CVIBuffer_Point_Instance* CVIBuffer_Point_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CVIBuffer_Instancing::INSTANCE_DESC& Desc)
{
	CVIBuffer_Point_Instance* pInstance = new CVIBuffer_Point_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(Desc)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Point_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent* CVIBuffer_Point_Instance::Clone(void* pArg)
{
	CVIBuffer_Point_Instance* pInstance = new CVIBuffer_Point_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_Point_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Point_Instance::Free()
{
	__super::Free();
	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pSpeed);
		Safe_Delete_Array(m_pInstanceVertices);
	}

}
