#pragma once

#include "VIBuffer.h"

/* �� = �޽� + �޽� + �޽� .. */
/* �޽÷� �����ϴ� ���� : ������ ��ü�� �����ϰ� ������ֱ� ���ؼ�.*/
/* �޽� = �������� + �ε��� */

BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& Prototype);
	virtual ~CMesh() = default;	

public:
	virtual HRESULT Initialize_Prototype(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;

private:
	_uint				m_iMaterialIndex = { 0 };

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END