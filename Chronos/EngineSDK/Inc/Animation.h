#pragma once

#include "Base.h"
BEGIN(Engine)
class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& Prototype);
	virtual ~CAnimation() = default;
public:
	HRESULT Initialize(ifstream* infile, const class CModel* pModel, _uint iBoneNum);
	_bool Update_TransformationMatrices(const vector<class CBone*>& Bones, _bool isLoop, _float fTimeDelta);
	_bool Update_ChangeAnimation(CAnimation* pAnimation , const vector<class CBone*>& Bones,_float fTimeDelta);

private:
	/* �� �ִϸ��̼��� �����ϱ����� �ɸ��� ��ü �Ÿ�. */
	_double m_Duration = { 0 };
	/* �ִϸ��̼��� �ʴ� ����ӵ� */
	_double m_SpeedPerSec = { 0 };
	/* m_CurrentTrackPosition += m_SpeedPerSec * fTimeDelta */
	_double m_CurrentTrackPosition = { 0 };
	/* �� �ִϸ��̼��� ����ϴ� ���� ���� */
	_uint m_iNumChannels = { 0 };

	_float4 m_vPreTranslation = {};

	vector<_uint>				m_CurrentKeyFrameIndices = { 0 };
	vector<class CChannel*>		m_Channels;

public:
	static CAnimation* Create(ifstream* infile, const class CModel* pModel, _uint iBoneNum);
	CAnimation* Clone();
	virtual void Free() override;
};
END
