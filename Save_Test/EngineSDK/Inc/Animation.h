#pragma once

#include "Base.h"

BEGIN(Engine)
class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(const aiAnimation* pAIAnimation, ofstream* outfile);


private:
	/* �� �ִϸ��̼��� ����ϴ� ���� ���� */
	_uint						m_iNumChannels = { 0 };
	/* ���� ������ ����*/
	/* Channel : �� (�ð��� ���� �� ���� ���� ����� �����Ѵ�) */
	vector<class CChannel*>		m_Channels;

	/* �� �ִϸ��̼��� �����ϱ����� �Ÿ��� ��ü �Ÿ�. */
	_double						m_Duration = {};

	/* �ִϸ��̼��� �ʴ� ����ӵ� */
	_double						m_SpeedPerSec = {};

	/* m_CurrentTrackPosition += m_SpeedPerSec * fTimeDelta */
	_double						m_CurrentTrackPosition = {};


public:
	static CAnimation* Create(const aiAnimation* pAIAnimation, ofstream* outfile);
	virtual void Free();

};
END
