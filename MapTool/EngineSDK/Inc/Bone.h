#pragma once

#include "Base.h"

BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	const _char* Get_Name() const {
		return m_szName;
	}

	_matrix Get_CombinedTransformationMatrix() const {
		return XMLoadFloat4x4(&m_CombinedTransformationMatrix);
	}

public:
	HRESULT Initialize(const aiNode* pAIBone);

private:
	_char				m_szName[MAX_PATH] = {};

	/* �θ�������� ǥ���� �� ������ �������. */
	_float4x4			m_TransformationMatrix = {};

	/* �� ���� �������� ������� ( �� ������� * �θ��� ����������� ) */
	/* m_TransformationMatrix * Parent`s m_CombinedTransformationMatrix */
	_float4x4			m_CombinedTransformationMatrix = {};



public:
	static CBone* Create(const aiNode* pAIBone);
	virtual void Free() override;
};

END