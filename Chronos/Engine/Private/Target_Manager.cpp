#include "Target_Manager.h"
#include "RenderTarget.h"

CTarget_Manager::CTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice(pDevice)
    , m_pContext(pContext)
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CTarget_Manager::Initialize()
{
    return S_OK;
}

HRESULT CTarget_Manager::Add_RenderTarget(const wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
    if (nullptr != Find_RenderTarget(strTargetTag))
        return E_FAIL;

    CRenderTarget* pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, iWidth, iHeight, ePixelFormat, vClearColor);
    if (nullptr == pRenderTarget)
        return E_FAIL;

    m_RenderTargets.emplace(strTargetTag, pRenderTarget);

    return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag)
{
    CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
    if (nullptr == pRenderTarget)
        return E_FAIL;


    list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);

    if (nullptr == pMRTList)
    {
        list<CRenderTarget*>	MRTList;
        MRTList.push_back(pRenderTarget);
        m_MRTs.emplace(strMRTTag, MRTList);
    }
    else
        pMRTList->push_back(pRenderTarget);

    Safe_AddRef(pRenderTarget);

    return S_OK;
}
HRESULT CTarget_Manager::Begin_MRT(const _wstring& strMRTTag)
{
    list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);

    if (nullptr == pMRTList)
        return E_FAIL;

    m_pContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);    // 1 : ���� ����.

    ID3D11RenderTargetView* pRenderTargets[8] = { nullptr };    // 8 : �ִ�� �׸� �� �ִ� ���� Ÿ���� ����.

    _uint iIndex = { 0 };
    for (auto& pRenderTarget : *pMRTList)
        pRenderTargets[iIndex++] = pRenderTarget->Get_RTV();    // MRT�� ����Ǿ� �ִ� ���� Ÿ�ٵ� �ȿ� �ִ� ����Ÿ�ٺ並 ���� ������ �迭�� �� ������.

    m_pContext->OMSetRenderTargets(iIndex, pRenderTargets, m_pDepthStencilView);    // ������ �Ͻ� �ܾ���� ����Ÿ�ٺ���� ��ġ�� ������.
    // �׷��� �׵��� ������� �信�� �ȱ׷���.
    // �� ��� �̰� �׸��� ���̴ϱ�.

    return S_OK;
}

HRESULT CTarget_Manager::End_MRT()
{
    // ������ �ٸ��� �׸������ ������ ����� �ٽ� ����.
    m_pContext->OMSetRenderTargets(1, &m_pBackBufferView, m_pDepthStencilView);
    

    Safe_Release(m_pBackBufferView);
    Safe_Release(m_pDepthStencilView);

    return S_OK;
}

CRenderTarget* CTarget_Manager::Find_RenderTarget(const _wstring& strTargetTag)
{
    auto	iter = m_RenderTargets.find(strTargetTag);

    if (iter == m_RenderTargets.end())
        return nullptr;

    return iter->second;
}

list<class CRenderTarget*>* CTarget_Manager::Find_MRT(const _wstring& strMRTTag)
{
    auto	iter = m_MRTs.find(strMRTTag);

    if (iter == m_MRTs.end())
        return nullptr;

    return &iter->second;
}

CTarget_Manager* CTarget_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTarget_Manager* pInstance = new CTarget_Manager(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Create Failed : CTarget Manager"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTarget_Manager::Free()
{
    __super::Free();

    for (auto& Pair : m_MRTs)
    {
        for (auto& pRenderTarget : Pair.second)
            Safe_Release(pRenderTarget);
        Pair.second.clear();
    }

    m_MRTs.clear();

    for (auto& Pair : m_RenderTargets)
        Safe_Release(Pair.second);
    m_RenderTargets.clear();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
