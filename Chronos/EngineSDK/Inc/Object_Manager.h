#pragma once

#include "Base.h"

/* 객체들을 모아서 그룹(Layer)별로 저장한다. */
/* 모아 놓은 객체들의 반복적인 Update를 호출해준다. */
/* 모아 놓은 객체들의 반복적인 Render를 호출해준다.(x) : 객체들을 그리는 순서를 내가 따로 좀 지정을 해야할 필요가 있다. */

BEGIN(Engine)

class CObject_Manager final : public CBase
{
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	list<class CGameObject*> Get_GameObjects(_uint iLevelIndex, const _wstring& strLayerTag);
	class CGameObject* Get_GameObject(_uint iLevelIndex, const _wstring& strLayerTag, _uint iObjectIndex);
	size_t Get_ObjectSize(_uint iLevelIndex, const _wstring& strLayerTag);

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_Prototype(const _wstring& strPrototypeTag, class CGameObject* pPrototype);	
	HRESULT Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, void* pArg);
	HRESULT Priority_Update(_float fTimeDelta);
	HRESULT Update(_float fTimeDelta);
	HRESULT Late_Update(_float fTimeDelta);
	void Clear(_uint iLevelIndex);

public:
	class CLayer* Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag);
	class CComponent* Find_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex);
	HRESULT Create_Layer(_uint iLevelIndex, const _wstring& strLayerTag);
	void Clear_Layer(_uint iLevelIndex, const _wstring& strLayerTag);
	void Release_Object(_uint iLevelIndex, const _wstring& strLayerTag);
	
	CGameObject* Clone_GameObject(const _wstring& strPrototypeTag, void* pArg);
	class CComponent* Find_PartComponent(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex, _uint iPartObjIndex);

	HRESULT Add_PoolingObject_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, _uint iCount);
	class CLayer* Find_PoolingLayer(_uint iLevelIndex, const _wstring& strLayerTag);
	HRESULT Add_Object_From_Pooling(_uint iLevelIndex, const _wstring& strLayerTag, void* pArg);
	HRESULT Release_Object_ByIndex(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex);

private:
	/* 원형객체들을 보관한다. */
	map<const _wstring, class CGameObject*>		m_Prototypes;

	/* 레벨별로, 사본객체들을 그룹별로 모아서 보관한다. */
	_uint										m_iNumLevels = {};

	map<const _wstring, class CLayer*>*			m_pLayers = { nullptr };
	map<const _wstring, class CLayer*>*			m_pPoolingLayers = { nullptr };

	typedef map<const _wstring, class CLayer*>		LAYERS;

private:
	class CGameObject* Find_Prototype(const _wstring& strPrototypeTag);

public:
	static CObject_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END