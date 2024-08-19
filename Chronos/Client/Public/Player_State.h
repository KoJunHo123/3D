#pragma once

#include "State.h"
#include "Client_Defines.h"
#include "Transform.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTransform;
END

BEGIN(Client)
class CPlayer_State abstract : public CState
{
public:
	typedef struct : public CState::STATE_DESC
	{
		class CTransform* pTransformCom = { nullptr };
		class CShader* pShaderCom = { nullptr };
		class CModel* pModelCom = { nullptr };
		_float* pSpeed = { nullptr };
	}PLAYER_STATE_DESC;

protected:
	CPlayer_State();
	virtual ~CPlayer_State() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT StartState(void** pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT ExitState(void** pArg) override;

protected:
	_vector Get_Rotation(_matrix WorldMatrix, _vector vExist);
	void Play_Animation(_float fTimeDelta);

	void Look_CameraDir();

protected:
	class CTransform* m_pTransformCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };
	_float* m_pSpeed = { nullptr };

	ANIM_PLAYER m_ePlayerAnim = { PLAYER_ANIM_END };

	// �ִϸ��̼� �����
	_bool m_isFinished = { false };
	// �ִϸ��̼� ���ư��� ���� �Է� �ȹ޴� �뵵
	_bool m_bMotionLock = { false };

	// �ִϸ��̼��� ����� �� ��� ���� ����Ǿ�����.
	_float m_fChangeRate = { 0.f };

	// �ִϸ��̼� �׽�Ʈ
	_float4 m_vTest = {};

public:
	virtual void Free();
};
END