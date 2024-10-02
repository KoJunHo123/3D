#pragma once

#include "Base.h"

/* ��ü���� �����Ѵ�. */

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	list<class CGameObject*> Get_GameObjects() {
		return m_GameObjects;
	}

	size_t Get_ObjectSize() {
		return m_GameObjects.size();
	}

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	HRESULT Priority_Update(_float fTimeDelta);
	HRESULT Update(_float fTimeDelta);
	HRESULT Late_Update(_float fTimeDelta);

	class CGameObject* Get_GameObject(_uint iObjectIndex);

	class CComponent* Find_Component(const _wstring& strComponentTag, _uint iIndex);
	class CComponent* Find_PartComponent(const _wstring& strComponentTag, _uint iIndex, _uint iPartObjIndex);

	void Release_Object();
	void Clear();

private:
	list<class CGameObject*>			m_GameObjects;

public:
	static CLayer* Create();
	virtual void Free() override;
};

END