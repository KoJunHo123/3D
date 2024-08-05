#include "Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation, ofstream* outfile)
{
	m_Duration = pAIAnimation->mDuration;
	m_SpeedPerSec = pAIAnimation->mTicksPerSecond;

	/* �� �ִϸ��̼��� ����ϴ� ���� ���������Ѵ�. */
	m_iNumChannels = pAIAnimation->mNumChannels;

	outfile->write(reinterpret_cast<const _char*>(&m_Duration), sizeof(_double));
	outfile->write(reinterpret_cast<const _char*>(&m_SpeedPerSec), sizeof(_double));
	outfile->write(reinterpret_cast<const _char*>(&m_iNumChannels), sizeof(_uint));

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i], outfile);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.emplace_back(pChannel);
	}
	return S_OK;
}

CAnimation* CAnimation::Create(const aiAnimation* pAIAnimation, ofstream* outfile)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAIAnimation, outfile)))
	{
		MSG_BOX(TEXT("Failed to Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	__super::Free();

	for (auto& channel : m_Channels)
		Safe_Release(channel);
}
