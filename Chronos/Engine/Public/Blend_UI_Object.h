#pragma once

#include "BlendObject.h"

/* ���������� �ʿ��� �ټ��� ��ü�� ������ �� �ִ�.  */
/* ���������� �ʿ��� �����͵�� ����� �������ִ� ��Ȱ */
BEGIN(Engine)

class ENGINE_DLL CBlend_UI_Object abstract : public CBlendObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_float	fX{}, fY{}, fSizeX{}, fSizeY{};
	} UI_DESC;

protected:
	CBlend_UI_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBlend_UI_Object(const CBlend_UI_Object& Prototype);
	virtual ~CBlend_UI_Object() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual _bool On_MousePoint(POINT ptMouse);

protected:
	_float				m_fX{}, m_fY{}, m_fSizeX{}, m_fSizeY{};
	_float4x4			m_ViewMatrix{}, m_ProjMatrix{};
	_uint				m_iDepth = {};

	_float				m_fViewWidth{}, m_fViewHeight{};



public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual CGameObject* Pooling() = 0;
	virtual void Free() override;
	virtual void Return();
};

END