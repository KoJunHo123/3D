#include "../Public/KeyManager.h"

CKeyManager::CKeyManager()
{
}

HRESULT CKeyManager::Initialize()
{
    ::ZeroMemory(m_bKeyState, sizeof(m_bKeyState));

    return S_OK;
}

_bool CKeyManager::Key_Pressing(int _iKey)
{
    if (GetAsyncKeyState(_iKey) & 0x8000)
        return true;

    return false;
}

_bool CKeyManager::Key_Down(int _iKey)
{
    // ������ ���� ���� ����, ���� ���� ����

    if ((!m_bKeyState[_iKey]) && (GetAsyncKeyState(_iKey) & 0x8000))
    {
        m_bKeyState[_iKey] = !m_bKeyState[_iKey];
        return true;
    }
    else
    {
        m_bKeyState[_iKey] = GetAsyncKeyState(_iKey);
    }
    return false;
}

_bool CKeyManager::Key_Up(int _iKey)
{
    // ������ ���� ���� �ְ�, ����� ������ ���� ����

    if ((m_bKeyState[_iKey]) && !(GetAsyncKeyState(_iKey) & 0x8000))
    {
        m_bKeyState[_iKey] = !m_bKeyState[_iKey];
        return true;
    }

    // key ���� ����
    for (int i = 0; i < VK_MAX; ++i)
    {
        if ((!m_bKeyState[i]) && (GetAsyncKeyState(_iKey) & 0x8000))
            m_bKeyState[i] = !m_bKeyState[i];
    }

    return false;
}

CKeyManager* CKeyManager::Create()
{
    CKeyManager* pInstance = new CKeyManager();
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Create : CKeyManager"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CKeyManager::Free()
{
    __super::Free();
}
