#include "../Public/KeyManager.h"

CKeyManager::CKeyManager()
{
}

HRESULT CKeyManager::Initialize()
{
    ::ZeroMemory(m_bKeyState, sizeof(m_bKeyState));

    return S_OK;
}

void CKeyManager::Update()
{
    for (size_t i = 0; i < VK_MAX; ++i)
    {
        if (true == m_bKeyState[i] && !(GetAsyncKeyState(i) & 0x8000))
            m_bKeyState[i] = false;
        else if (false == m_bKeyState[i] && (GetAsyncKeyState(i) & 0x8000))
            m_bKeyState[i] = true;
    }
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

    if ((false == m_bKeyState[_iKey]) && (GetAsyncKeyState(_iKey) & 0x8000))
    {
        m_bKeyState[_iKey] = true;

        return true;
    }

    return false;
}

_bool CKeyManager::Key_Up(int _iKey)
{
    if (1 == _iKey)
    {
        cout << "����" << endl;
        cout << (GetAsyncKeyState(_iKey) & 0x8000) << endl;
        cout << m_bKeyState[_iKey] << endl;
    }


    // ������ ���� ���� �ְ�, ����� ������ ���� ����
    if ((true == m_bKeyState[_iKey]) && !(GetAsyncKeyState(_iKey) & 0x8000))
    {
        m_bKeyState[_iKey] = false;
        return true;
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
