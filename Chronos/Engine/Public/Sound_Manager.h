#pragma once

#include "Base.h"

BEGIN(Engine)

class CSound_Manager final : public CBase
{
private:
	typedef struct
	{
		_uint iSrcIndex;
		_uint iDstIndex;
		_float fMixSpeed;
	}CROSS_INDEX;

private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	HRESULT Initialize(_uint iMaxChannel);
	void Update(_float fTimeDelta);

public:
	void SoundPlay(TCHAR* pSoundKey, _uint iChannelID, _float fVolume, _float3 SoundPos, _float3 PlayerPos);
	void PlayBGM(TCHAR* pSoundKey, _uint iBGMChannel, _float fVolume);
	void StopSound(_uint iChannelID);
	void StopSoundSlowly(_uint iChannelID);
	void StopAll();
	void SetChannelVolume(_uint iChannelID, _float fVolume);
	void LoadSoundFile();
	_uint Get_Position(_uint iChannelID);
	void Set_Position(_uint iChannelID, _uint iPositionMS);
	_bool IsPlaying(_uint iChannelID);
	void Set_Frequency(_uint iChannelID, _float fFrequency);
	void CrossFade(_uint iSrcChannelID, _uint iDstChannelID, _float fMixSpeed, TCHAR* pDstSoundKey, _float3 SoundPos, _float3 PlayerPos);

private:
	// ���� ���ҽ� ������ ���� ��ü
	map<TCHAR*, Sound*>	m_mapSound;

	// ���� ,ä�� ��ü �� ��ġ�� �����ϴ� ��ü 
	System*				m_pSystem = { nullptr };

	// FMOD_CHANNEL : ����ϰ� �ִ� ���带 ������ ��ü 
	vector<Channel*>	m_vecChannel;
	_uint				m_iVersion = {};

	FMOD_VECTOR m_vPlayerPosition = {};

	vector<_uint>	m_StopSoundIndex;
	vector<CROSS_INDEX> m_CrossIndices;

public:
	static CSound_Manager* Create(_uint iMaxChannel);
	virtual void Free() override;
};

END