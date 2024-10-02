#pragma once

#include "Base.h"

/* 1. ���� �Ҵ��� ������ �ּҸ� ��� �ִ´�. */
/* 2. ���� �Ҵ��� ������ �����Ѵ�. */
/* 3. ���ο� ������ ��ü �� ���� ������ �ı����ֳ�. + ���� ������ �ڿ�(���ҽ���, ��ü��) �� �����Ѵ�. */

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	HRESULT Initialize();
	void Update(_float fTimeDelta);
	HRESULT Render();
	void Change_Level(class CLevel* pNextLevel);

private:
	class CLevel*			m_pCurrentLevel = { nullptr };
	class CLevel*			m_pNextLevel = { nullptr };

	class CGameInstance*	m_pGameInstance = { nullptr };

public:
	static CLevel_Manager* Create();
	virtual void Free() override;
};

END