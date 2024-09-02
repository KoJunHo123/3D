#pragma once

#include "State.h"
#include "Client_Defines.h"
#include "Transform.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTransform;
class CPartObject;
class CNavigation;
END

BEGIN(Client)
class CPlayer_State abstract : public CState
{
public:
	typedef struct : public CState::STATE_DESC
	{
		class CTransform* pTransformCom = { nullptr };
		class CNavigation* pNavigationCom = { nullptr };

		vector<class CPartObject*>* Parts = { nullptr };

		PLAYER_ANIM* pPlayerAnim = { nullptr };
		_float* pSpeed = { nullptr };
		_bool* pIsFinished = { nullptr };
		_float3* pCameraLook = { nullptr };
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
	void Look_CameraDir();

protected:
	class CTransform* m_pTransformCom = { nullptr };
	class CNavigation* m_pNavigationCom = { nullptr };

	vector<class CPartObject*>			m_Parts;

	// �ִϸ��̼� ���ư��� ���� �Է� �ȹ޴� �뵵
	_bool m_bMotionLock = { false };

	// ������ �����ؾ� �ϴ� ����.
	PLAYER_ANIM* m_pPlayerAnim = { nullptr };
	_float* m_pSpeed = { nullptr };
	_bool* m_pIsFinished = { nullptr };
	_float3* m_pCameraLook = { nullptr };

public:
	virtual void Free();
};
END