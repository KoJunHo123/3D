#include "..\Public\Object_Manager.h"

#include "Layer.h"
#include "GameObject.h"

CObject_Manager::CObject_Manager()
{
}


list<class CGameObject*> CObject_Manager::Get_GameObjects(_uint iLevelIndex, const _wstring& strLayerTag)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

	if (nullptr == pLayer)
		Create_Layer(iLevelIndex, strLayerTag);

	return Find_Layer(iLevelIndex, strLayerTag)->Get_GameObjects();
}

CGameObject* CObject_Manager::Get_GameObject(_uint iLevelIndex, const _wstring& strLayerTag, _uint iObjectIndex)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_GameObject(iObjectIndex);
}

size_t CObject_Manager::Get_ObjectSize(_uint iLevelIndex, const _wstring& strLayerTag)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

	if (nullptr == pLayer)
		Create_Layer(iLevelIndex, strLayerTag);

	return Find_Layer(iLevelIndex, strLayerTag)->Get_ObjectSize();
}

HRESULT CObject_Manager::Initialize(_uint iNumLevels)
{
	if (nullptr != m_pLayers)
		return E_FAIL;

	m_pLayers = new LAYERS[iNumLevels];
	m_pPoolingLayers = new LAYERS[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const _wstring & strPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == pPrototype)
		return E_FAIL;

	if (nullptr != Find_Prototype(strPrototypeTag))
	{
		Safe_Release(pPrototype);
		return E_FAIL;
	}

	m_Prototypes.emplace(strPrototypeTag, pPrototype);

	return S_OK;
}

HRESULT CObject_Manager::Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring & strLayerTag, const _wstring & strPrototypeTag, void * pArg)
{
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	/* �����ؾ��� ������ ���𤤴�. */
	CGameObject* pPrototype = Find_Prototype(strPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	/* �� ������ �����Ͽ� �纻 ��ü�� �����Ѵ�. */
	CGameObject* pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return E_FAIL;

	/* ��ü���� ���׾�� ��� �����ϰ� �־��ŵ� */
	/* �纻�� �߰��ϱ����� ���̾ ã��.*/
	CLayer*		pLayer = Find_Layer(iLevelIndex, strLayerTag);

	/* �� �� ���̾ ���µ�? */
	/* ���� �߰��Ϸ����ߴ� ���̾ ���������� == ó�� �߰��ϴ� ��ü����. */
	/* ���մ� ���̤ø� ��������. */
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		pLayer->Add_GameObject(pGameObject);
		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);		
	}
	else /* ���� �߰��Ϸ��� �ϴ� ���׾ ��������־���. */
		pLayer->Add_GameObject(pGameObject);

	return S_OK;
}




HRESULT CObject_Manager::Priority_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		/* LEVEL_STATIC�� ���̾��� ���� �Ҵ�� ������ ���̾�鸸 ��ȿ�ϰ� ����ִ� ��Ȳ�� �ɲ���. */
		for (auto& Pair : m_pLayers[i])
		{
			Pair.second->Priority_Update(fTimeDelta);	
		}	
	}

	return S_OK;
}

HRESULT CObject_Manager::Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		/* LEVEL_STATIC�� ���̾��� ���� �Ҵ�� ������ ���̾�鸸 ��ȿ�ϰ� ����ִ� ��Ȳ�� �ɲ���. */
		for (auto& Pair : m_pLayers[i])
			Pair.second->Update(fTimeDelta);
	}

	return S_OK;
}

HRESULT CObject_Manager::Late_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		/* LEVEL_STATIC�� ���̾��� ���� �Ҵ�� ������ ���̾�鸸 ��ȿ�ϰ� ����ִ� ��Ȳ�� �ɲ���. */
		for (auto& Pair : m_pLayers[i])
			Pair.second->Late_Update(fTimeDelta);
	}

	return S_OK;
}

void CObject_Manager::Clear(_uint iLevelIndex)
{
	for (auto& Pair : m_pLayers[iLevelIndex])
		Safe_Release(Pair.second);

	m_pLayers[iLevelIndex].clear();

	for (auto& Pair : m_pPoolingLayers[iLevelIndex])
		Safe_Release(Pair.second);

	m_pPoolingLayers[iLevelIndex].clear();
}

CComponent * CObject_Manager::Find_Component(_uint iLevelIndex, const _wstring & strLayerTag, const _wstring & strComponentTag, _uint iIndex)
{
	CLayer*	pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Find_Component(strComponentTag, iIndex);	
}

HRESULT CObject_Manager::Create_Layer(_uint iLevelIndex, const _wstring & strLayerTag)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
	}

	return S_OK;
}

void CObject_Manager::Clear_Layer(_uint iLevelIndex, const _wstring& strLayerTag)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if(nullptr != pLayer)
		pLayer->Clear();
}

void CObject_Manager::Release_Object(_uint iLevelIndex, const _wstring& strLayerTag)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
		return;

	pLayer->Release_Object();
}


CGameObject* CObject_Manager::Clone_GameObject(const _wstring& strPrototypeTag, void* pArg)
{
	CGameObject* pGameObject = Find_Prototype(strPrototypeTag);
	if (nullptr == pGameObject)
		return nullptr;

	CGameObject* pCloneObject = pGameObject->Clone(pArg);
	if (nullptr == pCloneObject)
		return nullptr;

	return pCloneObject;
}

CComponent* CObject_Manager::Find_PartComponent(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex, _uint iPartObjIndex)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Find_PartComponent(strComponentTag, iIndex, iPartObjIndex);
}


CGameObject * CObject_Manager::Find_Prototype(const _wstring & strPrototypeTag)
{
	auto	iter = m_Prototypes.find(strPrototypeTag);
	if(iter == m_Prototypes.end())
		return nullptr;

	return iter->second;
}

CLayer * CObject_Manager::Find_Layer(_uint iLevelIndex, const _wstring & strLayerTag)
{
	auto	iter = m_pLayers[iLevelIndex].find(strLayerTag);
	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

HRESULT CObject_Manager::Add_PoolingObject_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, _uint iCount)
{
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	CGameObject* pPrototype = Find_Prototype(strPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	for(_uint i = 0; i < iCount; ++i)
	{
		CGameObject* pGameObject = pPrototype->Pooling();
		if (nullptr == pGameObject)
			return E_FAIL;

		CLayer* pLayer = Find_PoolingLayer(iLevelIndex, strLayerTag);

		if (nullptr == pLayer)
		{
			pLayer = CLayer::Create();
			pLayer->Add_GameObject(pGameObject);
			m_pPoolingLayers[iLevelIndex].emplace(strLayerTag, pLayer);
		}
		else
			pLayer->Add_GameObject(pGameObject);
	}

	CLayer* pTestLayer = Find_PoolingLayer(iLevelIndex, strLayerTag);

	return S_OK;
}

CLayer* CObject_Manager::Find_PoolingLayer(_uint iLevelIndex, const _wstring& strLayerTag)
{
	auto	iter = m_pPoolingLayers[iLevelIndex].find(strLayerTag);
	if (iter == m_pPoolingLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

HRESULT CObject_Manager::Add_Object_From_Pooling(_uint iLevelIndex, const _wstring& strLayerTag, void* pArg)
{
	CLayer* pPoolingLayer = Find_PoolingLayer(iLevelIndex, strLayerTag);
	if (nullptr == pPoolingLayer)
		return E_FAIL;

	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
	}
	pLayer->Set_PoolingLayer(pPoolingLayer);

	CGameObject* pGameObject = pPoolingLayer->Get_FrontGameObject();

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pGameObject->Initialize(pArg)))
		return E_FAIL;

	pLayer->Add_GameObject(pGameObject);

	return S_OK;
}

HRESULT CObject_Manager::Release_Object_ByIndex(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	
	if (nullptr == pLayer)
		return E_FAIL;

	pLayer->Release_Object(iIndex);

	return S_OK;
}

CObject_Manager * CObject_Manager::Create(_uint iNumLevels)
{
	CObject_Manager*		pInstance = new CObject_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX(TEXT("Failed to Created : CObject_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CObject_Manager::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])		
			Safe_Release(Pair.second);
		m_pLayers[i].clear();		
	}
	Safe_Delete_Array(m_pLayers);

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		LAYERS Layer = m_pPoolingLayers[i];
		for (auto& Pair : Layer)
			Safe_Release(Pair.second);
		m_pPoolingLayers[i].clear();
	}
	Safe_Delete_Array(m_pPoolingLayers);

	for (auto& Pair : m_Prototypes)
		Safe_Release(Pair.second);
	m_Prototypes.clear();
}

