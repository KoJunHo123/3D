#include "Channel.h"
#include "Model.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(ifstream* infile, const class CModel* pModel)
{
	infile->read(reinterpret_cast<_char*>(m_szName), sizeof(_char) * MAX_PATH);
	infile->read(reinterpret_cast<_char*>(&m_iNumKeyFrames), sizeof(_uint));
	
	m_iBoneIndex = pModel->Get_BoneIndex(m_szName);

	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME			KeyFrame{};
		infile->read(reinterpret_cast<_char*>(&KeyFrame), sizeof(KEYFRAME));
		m_KeyFrames.push_back(KeyFrame);
	}

	return S_OK;
}
void CChannel::Update_TransformationMatrix(const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex, _double CurrentTrackPosition)
{
	if (0.0 == CurrentTrackPosition)
		*pCurrentKeyFrameIndex = 0;

	KEYFRAME LastKeyFrame = m_KeyFrames.back();

	_vector vScale, vRotation, vTranslation;

	// ������ �ʿ� ���� ���
	if (CurrentTrackPosition >= LastKeyFrame.TrackPosition)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);
	}
	// ������ �ʿ��� ���
	else
	{
		if (CurrentTrackPosition >= m_KeyFrames[*pCurrentKeyFrameIndex + 1].TrackPosition)	// ���� ���������� �Ѿ��
			++*pCurrentKeyFrameIndex;

		_vector vSourScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vScale);
		_vector vDestScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex+1].vScale);

		_vector vSourRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex].vRotation);;
		_vector vDestRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vRotation);;

		_vector vSourTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vTranslation), 1.f);
		_vector vDestTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vTranslation), 1.f);

		// ���� Ű�����Ӱ� ���� Ű������ ������ ���� �������� ���� ���ϱ�
		_double		Ratio = (CurrentTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].TrackPosition) / (m_KeyFrames[*pCurrentKeyFrameIndex + 1].TrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].TrackPosition);

		// ���� ���ִ� �Լ�
		vScale = XMVectorLerp(vSourScale, vDestScale, (_float)Ratio);
		// ���ʹϾ� ���� ���ִ� �Լ�
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, (_float)Ratio);
		vTranslation = XMVectorLerp(vSourTranslation, vDestTranslation, (_float)Ratio);
	}

	// �ϴ� �������� -> ���߿� �θ� ��� ���ϸ鼭 ���� �ö� ����.
	// ���� ��� ������ִ� �Լ�. ũ��, ����, ȸ��, ��ġ �־���.
	_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

	// �̷��� ���� ���� ����� �ش��ϴ� �ε����� ���� �־���.
	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);

}

_bool CChannel::Update_ChangeChannel(CChannel* pChannel, const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex, _double CurrentTrackPosition)
{
	_vector vScale{}, vRotation{}, vTranslation{};

	_vector vSourScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vScale);
	_vector vDestScale = XMLoadFloat3(&pChannel->m_KeyFrames[0].vScale);

	_vector vSourRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex].vRotation);;
	_vector vDestRotation = XMLoadFloat4(&pChannel->m_KeyFrames[0].vRotation);;

	_vector vSourTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vTranslation), 1.f);
	_vector vDestTranslation = XMVectorSetW(XMLoadFloat3(&pChannel->m_KeyFrames[0].vTranslation), 1.f);

	// �� ���� Ű�������� ���ļ� ������ ���ΰ�?
	_double InterFrame = 5.;

	// ���� Ű�����Ӱ� ���� Ű������ ������ ���� ���� ���� ���� ���ϱ�
	m_Ratio = (CurrentTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].TrackPosition) / InterFrame;

	// ���� ���ִ� �Լ�
	vScale = XMVectorLerp(vSourScale, vDestScale, (_float)m_Ratio);
	// ���ʹϾ� ���� ���ִ� �Լ�
	vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, (_float)m_Ratio);
	if (1 == m_iBoneIndex)
		vTranslation = vDestRotation;
	else 
		vTranslation = XMVectorLerp(vSourTranslation, vDestTranslation, (_float)m_Ratio);

	_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);


	// �̷��� ���� ���� ����� �ش��ϴ� �ε����� ���� �־���.
	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);

	if (m_Ratio > 1.)
		return true;

	return false;
}




CChannel* CChannel::Create(ifstream* infile, const class CModel* pModel)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(infile, pModel)))
	{
		MSG_BOX(TEXT("Failed To Create : CChannel"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChannel::Free()
{
	__super::Free();
}

