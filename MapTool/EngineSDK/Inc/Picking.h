#pragma once

#include "Base.h"

BEGIN(Engine)

class CPicking final : public CBase
{
private:
	CPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPicking() = default;

public:
	HRESULT Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	void Update(); /* ��������ǥ��(����Ʈ��)�� ���콺 ��ǥ�� ���ϰ� �� ���콺�� ��ǥ�� ���� �����̽�(���� �����̽�)���� ���� �� ���� ��ŭ ������. */
	void Transform_ToLocalSpace(const _matrix& WorldMatrix);
	_bool isPicked_InWorldSpace(const _fvector& vPointA, const _fvector& vPointB, const _fvector& vPointC, _vector* pOut);
	_bool isPicked_InLocalSpace(const _fvector& vPointA, const _fvector& vPointB, const _fvector& vPointC, _vector* pOut);
	_bool isPicked_InLocalSpace(const _fvector& vPointA, const _fvector& vPointB, const _fvector& vPointC, _vector* pOut, _float* pDist);

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };

	HWND						m_hWnd = {};
	_uint						m_iWinSizeX = {};
	_uint						m_iWinSizeY = {};

private:
	_float4						m_vRayPos = {};
	_float4						m_vRayDir = {};

private:
	_float4						m_vRayPos_InLocalSpace = {};
	_float4						m_vRayDir_InLocalSpace = {};

public:
	static CPicking* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	virtual void Free();
};

END