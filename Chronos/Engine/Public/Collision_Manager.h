#pragma once

#include "Base.h"

BEGIN(Engine)
class CCollision_Manager final : public CBase
{
private:
	CCollision_Manager();
	virtual ~CCollision_Manager() = default;
	
public:
	HRESULT Initialize();
	void Update();

public:
	// ���� ������ �� �����ְ�
	void Add_CollisionKeys(const _wstring strCollisionFirst, const _wstring strCollisionSecond) {
		m_CollisionKeysFirst.emplace_back(strCollisionFirst);
		m_CollisionKeysSecond.emplace_back(strCollisionSecond);
	}

	// ������Ʈ �ʱ�ȭ���� �־��ְ�
	void Add_Collider_OnLayers(const _wstring strCollisionKey, class CCollider* pCollider);
	void Erase_Collider(const _wstring strCollisionKey, class CCollider* pCollider);

private:
	vector<_wstring> m_CollisionKeysFirst;
	vector<_wstring> m_CollisionKeysSecond;
	map<const _wstring, list<class CCollider*>> m_ColliderLayers;

public:
	static CCollision_Manager* Create();
	virtual void Free();
};
END
