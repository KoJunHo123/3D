#pragma once

#include "Client_Defines.h"
#include "Item.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
END

BEGIN(Client)
class CPuzzle_Item final : public CItem
{
public:
	typedef struct : CItem::ITEM_DESC
	{

	}PUZZLEITEM_DESC;

private:
	CPuzzle_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPuzzle_Item(const CPuzzle_Item& Prototype);
	virtual ~CPuzzle_Item() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(const _float4x4& WorldMatrix);

public:
	virtual void Add_Item() override;
	virtual _bool Use_Item(class CPlayer* pPlayer) override;

private:
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };

	_uint m_iMaxCount = { 4 };

private:
	HRESULT Ready_Components();

public:
	static CPuzzle_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual CGameObject* Pooling() override;
	virtual void Free();
};
END
