#include "../Public/Sound_Manager.h"

CSound_Manager::CSound_Manager()
{
}

HRESULT CSound_Manager::Initialize(_uint iMaxChannel)
{
	if (FMOD_OK != System_Create(&m_pSystem))
		return E_FAIL;

	if (FMOD_OK != m_pSystem->getVersion(&m_iVersion))
		return E_FAIL;

	if (FMOD_OK != m_pSystem->init(32, FMOD_INIT_NORMAL, nullptr))
		return E_FAIL;

	m_vecChannel.resize(iMaxChannel, nullptr);

	return S_OK;
}

void CSound_Manager::Update(_float fTimeDelta)
{
	for (auto& iter = m_StopSoundIndex.begin(); iter != m_StopSoundIndex.end(); )
	{
		_float fVolume = { 0.f };
		m_vecChannel[*iter]->getVolume(&fVolume);

		fVolume -= fTimeDelta;
		m_vecChannel[*iter]->setVolume(fVolume);

		if (fVolume < 0.f)
		{
			m_vecChannel[*iter]->stop();
			iter = m_StopSoundIndex.erase(iter);
		}
		else
			++iter;
	}

	for (auto& iter = m_CrossIndices.begin(); iter != m_CrossIndices.end(); )
	{
		_float fSrcVolume = { 0.f };
		_float fDstVolume = { 0.f };
		
		m_vecChannel[iter->iSrcIndex]->getVolume(&fSrcVolume);
		m_vecChannel[iter->iDstIndex]->getVolume(&fDstVolume);

		fSrcVolume -= fTimeDelta * iter->fMixSpeed;
		fDstVolume += fTimeDelta * iter->fMixSpeed;

		if (fSrcVolume < 0.f)
		{
			m_vecChannel[iter->iSrcIndex]->stop();
			iter = m_CrossIndices.erase(iter);
		}
		else
		{
			m_vecChannel[iter->iSrcIndex]->setVolume(fSrcVolume);
			m_vecChannel[iter->iDstIndex]->setVolume(fDstVolume);
			++iter;
		}
	}
}

void CSound_Manager::SoundPlay(TCHAR* pSoundKey, _uint iChannelID, _float fVolume, _float3 SoundPos, _float3 PlayerPos)
{
	auto iter = m_mapSound.find(pSoundKey);
	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	FMOD_VECTOR SoundPosition = { SoundPos.x, SoundPos.y,SoundPos.z };
	m_vPlayerPosition = { PlayerPos.x, PlayerPos.y,PlayerPos.z };
	FMOD_VECTOR velocity = { 0.f, 0.f, 0.f };

	_bool isPlaying = false;

	if (m_vecChannel[iChannelID]->isPlaying(&isPlaying))
	{
		m_vecChannel[iChannelID]->setMode(FMOD_3D);
		m_pSystem->playSound(iter->second, 0, false, &m_vecChannel[iChannelID]);

		m_pSystem->set3DListenerAttributes(0, &m_vPlayerPosition, 0, 0, 0);
		m_vecChannel[iChannelID]->set3DAttributes(&SoundPosition, &velocity);
		m_vecChannel[iChannelID]->set3DMinMaxDistance(4.0f, .0f);
	}
	m_vecChannel[iChannelID]->setVolume(fVolume);
	m_pSystem->update();
	FMOD_RESULT;
}

void CSound_Manager::PlayBGM(TCHAR* pSoundKey, _uint iBGMChannel, _float fVolume)
{
	auto iter = m_mapSound.find(pSoundKey);
	iter = find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	m_pSystem->playSound(iter->second, 0, false, &m_vecChannel[iBGMChannel]);
	m_vecChannel[iBGMChannel]->setMode(FMOD_LOOP_NORMAL);
	m_vecChannel[iBGMChannel]->setVolume(fVolume);
	m_pSystem->update();
}

void CSound_Manager::StopSound(_uint iChannelID)
{
	m_vecChannel[iChannelID]->stop();
}

void CSound_Manager::StopSoundSlowly(_uint iChannelID)
{
	m_StopSoundIndex.emplace_back(iChannelID);
}

void CSound_Manager::StopAll()
{
	for (auto iter : m_vecChannel)
		iter->stop();
}

void CSound_Manager::SetChannelVolume(_uint iChannelID, _float fVolume)
{
	m_vecChannel[iChannelID]->setVolume(fVolume);
}

void CSound_Manager::LoadSoundFile()
{
	// _finddata_t : <io.h>���� �����ϸ� ���� ������ �����ϴ� ����ü
	_finddatai64_t fd;
	
	path strFilePath = "../../Client/Bin/Resources/SFX/";
	for (auto& entry : recursive_directory_iterator(strFilePath))
	{
		if (is_directory(entry.path())) {
			bool has_subdirectories = false;

			// ���� �׸� ��ȸ (���� ������ �ִ��� Ȯ��)
			for (const auto& sub_entry : directory_iterator(entry.path())) {
				if (is_directory(sub_entry.path())) {
					has_subdirectories = true; // ���� ������ ������ true
					break; // ���� ������ �ϳ��� ã���� �� �̻� Ȯ���� �ʿ� ����
				}
			}

			// ���� ������ ������ ���� ������ �����ϰ� ���
			if (!has_subdirectories) {
				std::cout << entry.path() << std::endl;
				string strFilePath = entry.path().string();
				strFilePath += "/*.ogg";

				// _findfirst : <io.h>���� �����ϸ� ����ڰ� ������ ��� ������ ���� ù ��° ������ ã�� �Լ�
				intptr_t handle = _findfirsti64(strFilePath.c_str(), &fd);

				if (handle == -1)
					continue;

				int iResult = 0;

				// D:\����ȯ\145��\DefaultWindow\SFX	: ���� ���

				string strPath = entry.path().string();
				strPath += "/";
				char szCurPath[128] = "";	 // ��� ���
				strcpy_s(szCurPath, strPath.c_str());
				char szFullPath[128] = "";

				while (iResult != -1)
				{
					strcpy_s(szFullPath, szCurPath);

					// "../Sound/" + "Success.wav"
					strcat_s(szFullPath, fd.name);
					// "../Sound/Success.wav"

					Sound* pSound = nullptr;

					FMOD_RESULT eRes = m_pSystem->createSound(szFullPath, FMOD_LOOP_OFF, 0, &pSound);

					if (eRes == FMOD_OK)
					{
						int iLength = (int)strlen(fd.name) + 1;

						TCHAR* pSoundKey = new TCHAR[iLength];
						ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

						// �ƽ�Ű �ڵ� ���ڿ��� �����ڵ� ���ڿ��� ��ȯ�����ִ� �Լ�
						MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pSoundKey, iLength);

						m_mapSound.emplace(pSoundKey, pSound);
					}
					//_findnext : <io.h>���� �����ϸ� ���� ��ġ�� ������ ã�� �Լ�, ���̻� ���ٸ� -1�� ����
					iResult = _findnexti64(handle, &fd);
				}

				m_pSystem->update();

				_findclose(handle);
			}
		}
	}
}

_uint CSound_Manager::Get_Position(_uint iChannelID)
{
	_uint iPosition = { 0 };
	m_vecChannel[iChannelID]->getPosition(&iPosition, FMOD_TIMEUNIT_MS);
	return iPosition;
}

void CSound_Manager::Set_Position(_uint iChannelID, _uint iPositionMS)
{
	m_vecChannel[iChannelID]->setPosition(iPositionMS, FMOD_TIMEUNIT_MS);
}

_bool CSound_Manager::IsPlaying(_uint iChannelID)
{
	_bool isPlaying = { false };
	m_vecChannel[iChannelID]->isPlaying(&isPlaying);
	return isPlaying;
}

void CSound_Manager::Set_Frequency(_uint iChannelID, _float fFrequency)
{
	_float fCurrentFrequency = { 0.f };
	m_vecChannel[iChannelID]->getFrequency(&fCurrentFrequency);
	m_vecChannel[iChannelID]->setFrequency(fCurrentFrequency * fFrequency);
}

void CSound_Manager::CrossFade(_uint iSrcChannelID, _uint iDstChannelID, _float fMixSpeed, TCHAR* pDstSoundKey, _float3 SoundPos, _float3 PlayerPos)
{
	CROSS_INDEX CrossIndex = { iSrcChannelID, iDstChannelID, fMixSpeed };
	m_CrossIndices.emplace_back(CrossIndex);
	SoundPlay(pDstSoundKey, iDstChannelID, 0.f, SoundPos, PlayerPos);
}

CSound_Manager* CSound_Manager::Create(_uint iMaxChannel)
{
	CSound_Manager* pInstance = new CSound_Manager();
	if (FAILED(pInstance->Initialize(iMaxChannel)))
	{
		MSG_BOX(TEXT("Failed to Created : CSound_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSound_Manager::Free()
{
	__super::Free();

	m_vecChannel.clear();

	for (auto& MyPair : m_mapSound)
	{
		delete[] MyPair.first;
		MyPair.second->release();
	}
	m_mapSound.clear();

	m_pSystem->release();
	m_pSystem->close();
}
