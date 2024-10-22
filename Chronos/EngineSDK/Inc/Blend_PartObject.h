#pragma once

/* �����̳� ������Ʈ�ȿ� �߰��� �� �ִ� �ϳ��� ��ǰ��ü���� �θ� Ŭ���� */
#include "BlendObject.h"

BEGIN(Engine)

class ENGINE_DLL CBlend_PartObject abstract : public CBlendObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		const _float4x4* pParentWorldMatrix = { nullptr };
	}PARTOBJ_DESC;
protected:
	CBlend_PartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBlend_PartObject(const CBlend_PartObject& Prototype);
	virtual ~CBlend_PartObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();


protected:
	/* m_pTransformCom->m_WorldMatrix * �θ��� ���� */
	const _float4x4* m_pParentMatrix = { nullptr };

	_float4x4				m_WorldMatrix = {};	// �̰� �ڱ� ���� ����� �� ��.

protected:
	HRESULT Bind_WorldMatrix(class CShader* pShader, const _char* pContantName);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual CGameObject* Pooling() = 0;
	virtual void Free() override;
	virtual void Return();
};

END