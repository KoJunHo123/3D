#include "..\Public\GameInstance.h"
#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Timer_Manager.h"
#include "Input_Device.h"
#include "Picking.h"
#include "KeyManager.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{

}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
	/* 그래픽 카드를 초기화하낟. */
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.isWindowsed, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;


	/* 사운드 카드를 초기화하낟. */

	/* 입력장치를 초기화하낟. */
	m_pInput_Device = CInput_Device::Create(hInst, EngineDesc.hWnd);
	if (nullptr == m_pInput_Device)
		return E_FAIL;

	m_pKey_Manager = CKeyManager::Create();
	if (nullptr == m_pKey_Manager)
		return E_FAIL;

	m_pPicking = CPicking::Create(*ppDevice, *ppContext, EngineDesc.hWnd, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY);
	if (nullptr == m_pPicking)
		return E_FAIL;

	/* 여러가지 매니져를 초기화하낟. */
	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pObject_Manager = CObject_Manager::Create(iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pComponent_Manager = CComponent_Manager::Create(iNumLevels);
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;


	return S_OK;
}



void CGameInstance::Update_Engine(_float fTimeDelta)
{
	/* 현재 키보드와 마우스의 상태를 받아올꺼야. */
	m_pInput_Device->Update();

	m_pPicking->Update();

	m_pObject_Manager->Priority_Update(fTimeDelta);

	m_pPipeLine->Update();

	m_pObject_Manager->Update(fTimeDelta);

	m_pObject_Manager->Late_Update(fTimeDelta);

	m_pLevel_Manager->Update(fTimeDelta);
}

HRESULT CGameInstance::Draw_Engine()
{

	m_pRenderer->Draw();

	return m_pLevel_Manager->Render();
}

HRESULT CGameInstance::Clear(_uint iLevelIndex)
{
	/* 특정 레벨을 위해 존재하는 자원들은 해당 레벨이 끝나면 삭제하자. */
	m_pObject_Manager->Clear(iLevelIndex);

	/* 컴포넌트 원형들도 레벨별로 관리했었다. */
	// m_pComponent_Manager->Clear(iLevelIndex);

	return S_OK;
}

void CGameInstance::Render_Begin()
{
	/*m_pGraphic_Device->Render_Begin();*/
	m_pGraphic_Device->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGraphic_Device->Clear_DepthStencil_View();


}

void CGameInstance::Render_End()
{
	/*m_pGraphic_Device->Render_End(hWnd);*/

	m_pGraphic_Device->Present();


}
#pragma region INPUT_DEVICE
_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	return m_pInput_Device->Get_DIKeyState(byKeyID);
}

_byte CGameInstance::Get_DIMouseState(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Get_DIMouseState(eMouse);
}

_long CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	return m_pInput_Device->Get_DIMouseMove(eMouseState);
}

#pragma endregion

#pragma region LEVEL_MANAGER

HRESULT CGameInstance::Change_Level(_uint iLevelIndex, CLevel* pNextLevel)
{
	return m_pLevel_Manager->Change_Level(iLevelIndex, pNextLevel);
}

#pragma endregion

#pragma region OBJECT_MANAGER

HRESULT CGameInstance::Add_Prototype(const _wstring& strPrototypeTag, CGameObject* pPrototype)
{
	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);
}


HRESULT CGameInstance::Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, void* pArg)
{
	return m_pObject_Manager->Add_CloneObject_ToLayer(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

CComponent* CGameInstance::Find_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex)
{
	return m_pObject_Manager->Find_Component(iLevelIndex, strLayerTag, strComponentTag, iIndex);
}

HRESULT CGameInstance::Create_Layer(_uint iLevelIndex, const _wstring& strLayerTag)
{
	return m_pObject_Manager->Create_Layer(iLevelIndex, strLayerTag);
}

HRESULT CGameInstance::Save_Layer(_uint iLevelIndex, const _wstring& strLayerTag, ofstream* pOutFile)
{
	return m_pObject_Manager->Save_Layer(iLevelIndex, strLayerTag, pOutFile);
}
void CGameInstance::Clear_Layer(_uint iLevelIndex, const _wstring& strLayerTag)
{
	m_pObject_Manager->Clear_Layer(iLevelIndex, strLayerTag);
}
void CGameInstance::Release_Object(_uint iLevelIndex, const _wstring& strLayerTag)
{
	m_pObject_Manager->Release_Object(iLevelIndex, strLayerTag);
}
CLayer* CGameInstance::Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag)
{
	return m_pObject_Manager->Find_Layer(iLevelIndex, strLayerTag);
}
vector<_wstring> CGameInstance::Get_PrototypeKeys()
{
	return m_pObject_Manager->Get_PrototypeKeys();
}
CGameObject* CGameInstance::Get_GameObject(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex)
{
	return m_pObject_Manager->Get_GameObject(iLevelIndex, strLayerTag, iIndex);
}
#pragma endregion

#pragma region COMPONENT_MANAGER

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CComponent* pPrototype)
{
	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);
}

CComponent* CGameInstance::Clone_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
}

#pragma endregion

#pragma region TIMER_MANAGER

HRESULT CGameInstance::Add_Timer(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

_float CGameInstance::Compute_TimeDelta(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}

#pragma endregion


#pragma region RENDERER

HRESULT CGameInstance::Add_RenderObject(CRenderer::RENDERGROUP eRenderGroupID, CGameObject* pRenderObject)
{
	return m_pRenderer->Add_RenderObject(eRenderGroupID, pRenderObject);
}
#pragma endregion

#pragma region PIPELINE
void CGameInstance::Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	return m_pPipeLine->Set_Transform(eState, TransformMatrix);
}
_float4x4 CGameInstance::Get_Transform_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Float4x4(eState);
}
_matrix CGameInstance::Get_Transform_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Matrix(eState);
}
_float4x4 CGameInstance::Get_Transform_Inverse_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Inverse_Float4x4(eState);
}
_matrix CGameInstance::Get_Transform_Inverse_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Inverse_Matrix(eState);
}

_float4 CGameInstance::Get_CamPosition_Float4() const
{
	return m_pPipeLine->Get_CamPosition_Float4();
}
_vector CGameInstance::Get_CamPosition_Vector() const
{
	return m_pPipeLine->Get_CamPosition_Vector();
}
#pragma endregion

#pragma region PICKING
void CGameInstance::Transform_MouseRay_ToLocalSpace(const _matrix& WorldMatrix)
{
	return m_pPicking->Transform_ToLocalSpace(WorldMatrix);
}
_bool CGameInstance::isPicked_InWorldSpace(const _fvector& vPointA, const _fvector& vPointB, const _fvector& vPointC, _vector* pOut)
{
	return m_pPicking->isPicked_InWorldSpace(vPointA, vPointB, vPointC, pOut);
}
_bool CGameInstance::isPicked_InLocalSpace(const _fvector& vPointA, const _fvector& vPointB, const _fvector& vPointC, _vector* pOut)
{
	return m_pPicking->isPicked_InLocalSpace(vPointA, vPointB, vPointC, pOut);
}
_bool CGameInstance::isPicked_InLocalSpace(const _fvector& vPointA, const _fvector& vPointB, const _fvector& vPointC, _vector* pOut, _float* pDist)
{
	return m_pPicking->isPicked_InLocalSpace(vPointA, vPointB, vPointC, pOut, pDist);
}
#pragma endregion

#pragma region KEY_MANAGER
_bool CGameInstance::Key_Pressing(int _iKey)
{
	return m_pKey_Manager->Key_Pressing(_iKey);
}
_bool CGameInstance::Key_Down(int _iKey)
{
	return m_pKey_Manager->Key_Down(_iKey);
}
_bool CGameInstance::Key_Up(int _iKey)
{
	return m_pKey_Manager->Key_Up(_iKey);
}
#pragma endregion

void CGameInstance::Release_Engine()
{
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pPicking);
	Safe_Release(m_pKey_Manager);
	Safe_Release(m_pGraphic_Device);

	CGameInstance::Get_Instance()->Destroy_Instance();
}

void CGameInstance::Free()
{
	__super::Free();

}
