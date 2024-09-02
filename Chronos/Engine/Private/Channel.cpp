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
void CChannel::Update_TransformationMatrix(const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex, _double CurrentTrackPosition, _bool isChange, _double RatioMax)
{
	if (0.0 == CurrentTrackPosition)
		*pCurrentKeyFrameIndex = 0;

	_vector vScale, vRotation, vTranslation;

	_vector vSourScale{}, vSourRotation{}, vSourTranslation{};
	_vector vDestScale{}, vDestRotation{}, vDestTranslation{};

	_double	Ratio = 0.f;

	if (true == isChange)
	{
		if(0. == CurrentTrackPosition)
		{
			XMMatrixDecompose(&vSourScale, &vSourRotation, &vSourTranslation, Bones[m_iBoneIndex]->Get_TransformationMatrix());

			XMStoreFloat4(&m_vSourScale, vSourScale);
			XMStoreFloat4(&m_vSourRotation, vSourRotation);
			XMStoreFloat4(&m_vSourTranslation, vSourTranslation);
		}

		vSourScale = XMLoadFloat4(&m_vSourScale);
		vSourRotation = XMLoadFloat4(&m_vSourRotation);
		vSourTranslation = XMLoadFloat4(&m_vSourTranslation);

		vDestScale			= XMLoadFloat3(&m_KeyFrames[0].vScale);
		vDestRotation		= XMLoadFloat4(&m_KeyFrames[0].vRotation);
		vDestTranslation	= XMVectorSetW(XMLoadFloat3(&m_KeyFrames[0].vTranslation), 1.f);

		// ���� Ű�����Ӱ� ���� Ű������ ������ ���� �������� ���� ���ϱ�
		Ratio = CurrentTrackPosition / RatioMax;
	}
	else
	{
		KEYFRAME LastKeyFrame = m_KeyFrames.back();

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
			while (CurrentTrackPosition >= m_KeyFrames[*pCurrentKeyFrameIndex + 1].TrackPosition)	// ���� ���������� �Ѿ��
				++*pCurrentKeyFrameIndex;

			vSourScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vScale);
			vDestScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vScale);

			vSourRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex].vRotation);;
			vDestRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vRotation);;

			vSourTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vTranslation), 1.f);
			vDestTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vTranslation), 1.f);

			// ���� Ű�����Ӱ� ���� Ű������ ������ ���� �������� ���� ���ϱ�
			Ratio = (CurrentTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].TrackPosition) / (m_KeyFrames[*pCurrentKeyFrameIndex + 1].TrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].TrackPosition);
		}
	}

	// ���� ���ִ� �Լ�
	vScale = XMVectorLerp(vSourScale, vDestScale, (_float)Ratio);
	// ���ʹϾ� ���� ���ִ� �Լ�
	vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, (_float)Ratio);
	vTranslation = XMVectorLerp(vSourTranslation, vDestTranslation, (_float)Ratio);

	// �ϴ� �������� -> ���߿� �θ� ��� ���ϸ鼭 ���� �ö� ����.
	// ���� ��� ������ִ� �Լ�. ũ��, ����, ȸ��, ��ġ �־���.
	_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

	// �̷��� ���� ���� ����� �ش��ϴ� �ε����� ���� �־���.
	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
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

